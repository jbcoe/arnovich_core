/*
 * @file srv_server.c
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <core_srv/core_srv_server.h>
#include <core_srv/core_srv_data.h>

#include <pthread.h>

#include <sys/types.h>
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
#endif

#include <stdio.h>
#include <errno.h>


#ifdef DEBUG

#define _DEBUG(exp, ...) \
	{\
		printf("%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	};

#else

#define _DEBUG(exp, ...) ;

#endif

static struct srv_tickers *the_tickers = NULL;

#define _ERROR(errstr)\
    if(nelem < 0){\
        _DEBUG( \
            "%s:  err=%i  errno=%i\n", errstr, (int)nelem, errno); \
        break; \
    } else if(nelem == 0){\
        _DEBUG( \
            "%s: CLIENT CLOSED CONNECTION", errstr); \
        break; \
    } \

#define _ERROR_RTN(err, errstr)\
    nelem = write(connection, (void*)&err, sizeof(SRV_ERROR)); \
    if(nelem < 0){\
        _DEBUG( \
            "FAILED TO WRITE ERROR RETURN = %i  %i\n", (int)nelem, errno); \
        break; \
    } else if(nelem == 0){\
        _DEBUG( \
            "FAILED TO WRITE ERROR RETURN: CLIENT CLOSED CONNECTION"); \
        break; \
    } \
    if(err != SRV_ERROR_SUCCESS) { \
        _DEBUG( \
            "RETURNING ERROR TO CLIENT:  code=%i  errstr=%s", err, errstr); \
        break; \
    }

void* srv_run_server_thread(void *vconnection)
{
    int connection = *((int*)(vconnection));
    SRV_COMMANDS command = -1;
    char ticker[SRV_TICKER_LEN];
    size_t nelem;
    int id;
    SRV_ERROR rtn;
    SRV_ERROR err;
    void *tickdata;
    size_t len;
    struct timeval datetime;
    struct timeval last_pull;
    struct srv_subscriber* subscr;
    int i;

    last_pull.tv_sec = 0;
    last_pull.tv_usec = 0;
    while(1)
    {
        _DEBUG( \
        	"WAITING FOR READ %i", connection)
        nelem = read(connection, &command, sizeof(SRV_COMMANDS));
		_ERROR("COMMAND READ IS WRONG");

        switch(command) {
        case SRV_COMMANDS_OPEN_CONNECTION:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_OPEN_CONNECTION FROM %i", connection);
            break;
        case SRV_COMMANDS_CLOSE_CONNECTION:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_CLOSE_CONNECTION FROM %i", connection);
            _DEBUG("CLOSING THREAD BY DESIGN for %i", connection);
            return NULL;
        case SRV_COMMANDS_ADD_TICKER:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_ADD_TICKER FROM %i", connection);
            nelem = read(connection, (void*)ticker, sizeof(char)*SRV_TICKER_LEN);
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER = %s FROM %i", ticker, connection);
            rtn = srv_add_ticker(the_tickers, ticker, &id);
            _ERROR_RTN(rtn, "SRV_COMMANDS_ADD_TICKER: srv_add_ticker error")
            _DEBUG("WRITING RESPONSE");
            nelem = write(connection, (void*)&id, sizeof(int));
            _DEBUG("TICKER ID WRITE FAILED");
            break;
        case SRV_COMMANDS_GET_TICKER:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_GET_TICKER FROM %i", connection);
            nelem = read(connection, (void*)ticker, sizeof(char)*SRV_TICKER_LEN);
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER = %s FROM %i", ticker, connection);
            rtn = srv_get_ticker(the_tickers, ticker, &id);
            _ERROR_RTN(rtn, "SRV_COMMANDS_GET_TICKER: srv_get_ticker error")
            nelem = write(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID WRITE FAILED");
            break;
        case SRV_COMMANDS_PUSH_TICK:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_PUSH_TICK FROM %i", connection);
            nelem = read(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER ID = %i FROM %i", id, connection);
            nelem = read(connection, (void*)&len, sizeof(size_t));
            _ERROR("TICKER DATA LEN READ IS WRONG")
            _DEBUG("RECEIVED TICKER DATA LEN = %i FROM %i", (int)len, connection);
            tickdata = (void*)malloc(len);
            i=len/SRV_BUFFER_LEN;
            nelem = read(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*i), len%SRV_BUFFER_LEN);
            //_DEBUG("RECEIVED TICKER DATA = %s", (char*)(tickdata+SRV_BUFFER_LEN*i));
            _ERROR("TICKER DATA READ IS WRONG")
            err = SRV_ERROR_SUCCESS;
            nelem = write(connection, (void*)&err, sizeof(SRV_ERROR));
            for(; i>0; --i) {
                nelem = read(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
                _ERROR("TICKER DATA READ IS WRONG")
				//_DEBUG("RECEIVED TICKER DATA = %s", (char*)(tickdata+SRV_BUFFER_LEN*(i-1)));
                err = SRV_ERROR_SUCCESS;
                nelem = write(connection, (void*)&err, sizeof(SRV_ERROR));
            }
            _DEBUG("RECEIVED TICKER DATA = %s", (char*)tickdata);
            rtn = srv_push_tick(the_tickers, id, tickdata, len);
            _ERROR_RTN(rtn, "SRV_COMMANDS_PUSH_TICK: srv_push_tick error")
            _DEBUG("PUSHED TICK ON TICKERS");
            pthread_mutex_lock(&the_tickers->m_tickers[id]->m_subscriblock);
            subscr = the_tickers->m_tickers[id]->m_subscribers;
            command = SRV_COMMANDS_NOTIFY_SUBSCRIBER;
            while(subscr != NULL) {
                _DEBUG("COMMAND = SRV_COMMANDS_NOTIFY_SUBSCRIBER TO %i", subscr->m_connection);
            	nelem = write(subscr->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
            	_ERROR("SUBSCRIPTION WRITE FAILED");
                nelem = write(subscr->m_connection, (void*)&len, sizeof(size_t));
                _ERROR("SUBSCRIPTION TICKER DATA LEN WRITE FAILED");
                i=len/SRV_BUFFER_LEN;
                nelem = write(subscr->m_connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*i), len%SRV_BUFFER_LEN);
                _ERROR("SUBSCRIPTION TICKER DATA DATA WRITE FAILED");
                for(; i>0; --i) {
                    nelem = write(subscr->m_connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
                    _ERROR("SUBSCRIPTION TICKER DATA DATA WRITE FAILED");
                }
				//TODO fix datetime - it is not initialised!
                nelem = write(subscr->m_connection, (void*)&datetime, sizeof(struct timeval));
                _ERROR("SUBSCRIPTION TICKER DATA DATETIME WRITE FAILED");
            	subscr = subscr->m_next;
            }
            pthread_mutex_unlock(&the_tickers->m_tickers[id]->m_subscriblock);
            free(tickdata);
            break;
        case SRV_COMMANDS_PULL_TICK:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_PULL_TICK FROM %i", connection);
            nelem = read(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER ID = %i FROM %i", id, connection);
            tickdata = NULL;
            rtn = srv_pull_tick(the_tickers, id, &tickdata, &len, &datetime);
            _ERROR_RTN(rtn, "SRV_COMMANDS_PULL_TICK: srv_pull_tick error")
            nelem = write(connection, (void*)&len, sizeof(size_t));
            _ERROR("TICKER DATA LEN WRITE FAILED");
            _DEBUG("TICKER DATA WROTE  len= %i   data=%s", (int)len, (char*)tickdata);

            i=len/SRV_BUFFER_LEN;
            nelem = write(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*i), len%SRV_BUFFER_LEN);
            _ERROR("TICKER DATA DATA WRITE FAILED");
            err = SRV_ERROR_SUCCESS;
            nelem = read(connection, (void*)&err, sizeof(SRV_ERROR));
            _ERROR("TICKER DATA STATUS READ FAILED");
            if(err != SRV_ERROR_SUCCESS) {
                _DEBUG( \
                    "READ STATUS ERROR:  err=%i", err); \
                break;
            }
            for(; i>0; --i) {
                nelem = write(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
                _ERROR("TICKER DATA DATA WRITE FAILED");
                nelem = read(connection, (void*)&err, sizeof(SRV_ERROR));
                _ERROR("TICKER DATA STATUS READ FAILED");
                if(err != SRV_ERROR_SUCCESS) {
                    _DEBUG( \
                        "READ STATUS ERROR:  err=%i", err); \
                    break;
                }
            }
            nelem = write(connection, (void*)&datetime, sizeof(struct timeval));
            _ERROR("TICKER DATA DATETIME WRITE FAILED");
            last_pull = datetime;
            _DEBUG("TICKER DATA DATETIME WROTE  datetime=%i:%i", (int)datetime.tv_sec, (int)datetime.tv_usec);
            _DEBUG("PULLED AND WROTE DATA");
            break;
		//remove: also remove all ->next in tick's
        case SRV_COMMANDS_PULL_TICK_BY_DATETIME:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_PULL_TICK_BY_DATETIME FROM %i", connection);
            nelem = read(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER ID = %i FROM %i", id, connection);
            nelem = read(connection, (void*)&datetime, sizeof(struct timeval));
            _ERROR("TICKER DATETIME READ IS WRONG");
            _DEBUG("RECEIVED TICKER DATETIME = %i:%i FROM %i", (int)datetime.tv_sec, (int)datetime.tv_usec, connection);
            tickdata = NULL;
            rtn = srv_pull_tick_by_datetime(the_tickers, id, datetime, &tickdata, &len, &datetime);
            _ERROR_RTN(rtn, "NO TICK FOUND")
            _DEBUG("TICKER DATA WRITING  len= %i   data=%s", (int)len, (char*)tickdata);
            nelem = write(connection, (void*)&len, sizeof(size_t));
            _ERROR("TICKER DATA LEN WRITE FAILED");
            _DEBUG("TICKER DATA WROTE  len= %i   data=%s", (int)len, (char*)tickdata);

            i=len/SRV_BUFFER_LEN;
            nelem = write(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*i), len%SRV_BUFFER_LEN);
            _ERROR("TICKER DATA DATA WRITE FAILED");
            for(; i>0; --i) {
                _DEBUG("TICKER DATA DATETIME WROTE  datetime=%i:%i", (int)datetime.tv_sec, (int)datetime.tv_usec);
                nelem = write(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
                _ERROR("TICKER DATA DATA WRITE FAILED");
            }
            nelem = write(connection, (void*)&datetime, sizeof(struct timeval));
            _ERROR("TICKER DATA DATETIME WRITE FAILED");
            if((datetime.tv_sec > last_pull.tv_sec) ||
               ((datetime.tv_sec == last_pull.tv_sec) &&
                (datetime.tv_usec > last_pull.tv_usec))){
                last_pull = datetime;
            }
            _DEBUG("TICKER DATA DATETIME WROTE  datetime=%i:%i", (int)datetime.tv_sec, (int)datetime.tv_usec);
            _DEBUG("PULLED AND WROTE DATA");
            break;
        case SRV_COMMANDS_PULL_TICK_UPDATE:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_PULL_TICK_UPDATE FROM %i", connection);
            nelem = read(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER ID = %i FROM %i", id, connection);
            tickdata = NULL;
            datetime = last_pull;
            datetime.tv_usec += 1;
            _DEBUG("ABOUT TO PULL WITH TICK DATETIME  datetime=%i:%i", (int)datetime.tv_sec, (int)datetime.tv_usec);
            rtn = srv_pull_tick_update(the_tickers, id, datetime, &tickdata, &len, &datetime);
            _ERROR_RTN(rtn, "NO NEW TICK UPDATE AVAILABLE")
            nelem = write(connection, (void*)&len, sizeof(size_t));
            _ERROR("TICKER DATA LEN WRITE FAILED");

            i=len/SRV_BUFFER_LEN;
            nelem = write(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*i), len%SRV_BUFFER_LEN);
            _ERROR("TICKER DATA DATA WRITE FAILED");
            for(; i>0; --i) {
                _DEBUG("TICKER DATA DATETIME WROTE  datetime=%i:%i", (int)datetime.tv_sec, (int)datetime.tv_usec);
                nelem = write(connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
                _ERROR("TICKER DATA DATA WRITE FAILED");
            }
            nelem = write(connection, (void*)&datetime, sizeof(struct timeval));
            _ERROR("TICKER DATA DATETIME WRITE FAILED");
            last_pull = datetime;
            _DEBUG("TICKER DATA DATETIME WROTE  datetime=%i:%i", (int)datetime.tv_sec, (int)datetime.tv_usec);
            _DEBUG("PULLED AND WROTE DATA");
            break;
        case SRV_COMMANDS_SUBSCRIBE_TO_TICKER:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_SUBSCRIBE_TO_TICKER FROM %i", connection);
            nelem = read(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER ID = %i FROM %i", id, connection);
            rtn = srv_tick_subscribe(the_tickers, id, connection);
            _ERROR_RTN(rtn, "SUBSCRIPTION FAILED")
            _DEBUG("SUBSCRIBED TO DATA ID=%i FOR CONNECTION=%i", id, connection);
            break;
        case SRV_COMMANDS_UNSUBSCRIBE_TO_TICKER:
            _DEBUG("RECEIVED COMMAND = SRV_COMMANDS_UNSUBSCRIBE_TO_TICKER FROM %i", connection);
            nelem = read(connection, (void*)&id, sizeof(int));
            _ERROR("TICKER ID READ IS WRONG");
            _DEBUG("RECEIVED TICKER ID = %i FROM %i", id, connection);
            rtn = srv_tick_unsubscribe(the_tickers, id, connection);
            _ERROR_RTN(rtn, "UNSUBSCRIPTION FAILED")
            _DEBUG("UNSUBSCRIBED TO DATA ID=%i FOR CONNECTION=%i", id, connection);
            break;
        default:
            _DEBUG("RECEIVED UNKNOWN COMMAND! %i FROM %i", command, connection);
            break;
        }
    }

    _DEBUG("CLOSING THREAD BY ACCIDENT for %i", connection);
    return NULL;
}


SRV_ERROR srv_run_server(SRV_SOCKET_TYPES socket_type)
{
    int s, ns;

    #ifndef _WIN32
    struct sockaddr_un saun, fsaun;
    #else
    struct sockaddr fsaun;
    #endif

    struct sockaddr_in isa;
    memset(&isa, 0, sizeof(isa));
    struct protoent* tcp;

    struct sockaddr* saddr, *client_addr;
    int saddr_len, client_addr_len;
    int protocol;
    int addr;

    switch(socket_type) {
    case SRV_SOCKET_TYPES_LOCAL_PIPES:
        // if connect using files:
        // only support on non-win32
        #ifdef _WIN32
        return SRV_ERROR_UNKNOWN;
		#endif
    	addr = AF_UNIX;
    	protocol = 0;

    	memset(&saun, 0, sizeof(saun));
    	//only if OSX/FREEBSD
    	//saun.sun_len = sizeof(saun);
        saun.sun_family = AF_UNIX;
        strcpy(saun.sun_path, SRV_LOCAL_PIPES_SERVER_ADDRESS);
        unlink(SRV_LOCAL_PIPES_SERVER_ADDRESS);

    	saddr = (struct sockaddr*)&saun;
    	saddr_len = sizeof(saun);

    	client_addr = (struct sockaddr*)&fsaun;
    	client_addr_len = sizeof(fsaun);
    	break;
    case SRV_SOCKET_TYPES_TCP_IP:
    	//using tcp on localhost
    	addr = PF_INET;
    	tcp = getprotobyname("tcp");
    	protocol = tcp->p_proto;

    	isa.sin_family = AF_INET;
    	isa.sin_port = htons(SRV_TCP_IP_SERVER_PORT);

    	struct hostent* localhost = gethostbyname(SRV_TCP_IP_SERVER_ADDRESS);
    	memcpy(&isa.sin_addr.s_addr, localhost->h_addr_list[0], sizeof(localhost->h_length));

    	saddr = (struct sockaddr*)&isa;
    	saddr_len = sizeof(isa);

    	client_addr = (struct sockaddr*)&fsaun;
    	client_addr_len = sizeof(fsaun);
    	break;
    default:
        return SRV_ERROR_UNKNOWN;
    }

    _DEBUG( \
    	"Socket...")
	if((s = socket(addr, SOCK_STREAM, protocol)) < 0)
	{
		 perror("server: socket");
		 return SRV_ERROR_UNKNOWN;
	}

    //turn-on Nagle algo for TCP
	//int v = 1;
    //win32/osx
	//setsockopt(addr, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
    //linux
	//setsockopt(addr, SOL_TCP, TCP_NODELAY, &v, sizeof(v));

    _DEBUG( \
    	"Port Binding...")
    if(bind(s, saddr, saddr_len) < 0)
    {
        perror("server: bind");
        close(s);
        return SRV_ERROR_UNKNOWN;
    }

    _DEBUG( \
    	"Listening for connections...")
    if(listen(s, 10) < 0)
    {
        perror("server: listen");
        close(s);
        return SRV_ERROR_UNKNOWN;
    }

    srv_data_init_tickers(&the_tickers);
    while(1)
    {
        _DEBUG( \
        	"Accepting for connections...")

        // check for new connection
        if((ns = accept(s, client_addr, (socklen_t*)&client_addr_len)) < 0)
        {
            perror("server: accept");
            close(s);
            return SRV_ERROR_UNKNOWN;
        }

        pthread_t dummy;
        _DEBUG( \
        	"Connection accepted: Connection thread started on %i", ns)
        pthread_create(&dummy, NULL, srv_run_server_thread, &ns);
    }

    close(s);
    return SRV_ERROR_SUCCESS;
}
