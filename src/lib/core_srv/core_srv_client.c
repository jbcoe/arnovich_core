/**
  * @file core_srv_client.c
  * @brief Implementation file for server client.
  */

#include <core_srv/core_srv_client.h>

#include <sys/types.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
int write(SOCKET s, void *buf, int len)
{
    return send(s, (const char*)buf, len, MSG_DONTROUTE);
}
int close(SOCKET s)
{
    return closesocket(s);
}
int read(SOCKET s, void *buf, int len)
{
    return recv(s, (char*)buf, len, MSG_WAITALL);
}
#endif

#ifdef DEBUG

#define _DEBUG(exp, ...) \
	{\
		printf("%s:%i   "exp"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	};

#else

#define _DEBUG(exp, ...) ;

#endif


SRV_ERROR srv_client_connect(struct srv_client_connection ** connection, SRV_SOCKET_TYPES socket_type)
{
    #ifndef _WIN32
    struct sockaddr_un saun;
    #endif
    struct sockaddr_in isa;
    struct sockaddr* saddr;
    int addr;
    int protocol;
    int saddr_len;
    struct protoent* tcp;
    struct hostent* localhost;
    SRV_COMMANDS command = SRV_COMMANDS_OPEN_CONNECTION;
    size_t r;

    switch(socket_type) {
    case SRV_SOCKET_TYPES_LOCAL_PIPES:
       	// if connect using files:
        // only support on non-win32
        #ifdef _WIN32
       	return SRV_ERROR_UNKNOWN;
        #else
       	addr = AF_UNIX;
       	protocol = 0;

    	memset(&saun, 0, sizeof(saun));
    	//only if OSX/FREEBSD
    	//saun.sun_len = sizeof(saun);
       	saun.sun_family = addr;
       	strcpy(saun.sun_path, SRV_LOCAL_PIPES_SERVER_ADDRESS);

       	saddr = (struct sockaddr*)&saun;
       	saddr_len = sizeof(saun);
        #endif
       	break;
    case SRV_SOCKET_TYPES_TCP_IP:
    	//using tcp on localhost
       	addr = AF_INET;
       	tcp = getprotobyname("tcp");
       	protocol = tcp->p_proto;

       	isa.sin_family = AF_INET;
       	isa.sin_port = htons(SRV_TCP_IP_SERVER_PORT);

       	localhost = gethostbyname(SRV_TCP_IP_SERVER_ADDRESS);
       	memcpy(&isa.sin_addr.s_addr, localhost->h_addr_list[0], sizeof(localhost->h_length));

       	saddr = (struct sockaddr*)&isa;
       	saddr_len = sizeof(isa);
       	break;
    default:
        return SRV_ERROR_UNKNOWN;
    }

    *connection =
            (struct srv_client_connection *)malloc(sizeof(struct srv_client_connection));

    if (((*connection)->m_connection = socket(addr, SOCK_STREAM, protocol)) < 0) {
        perror("client: socket");
        _DEBUG(
            "client socket failed %i", errno);
        return SRV_ERROR_CONNNECTION_ERROR;
    }

    //int v = 1;
    //setsockopt((*connection)->m_connection, SOL_TCP, TCP_NODELAY, &v, sizeof(v));

    if (connect((*connection)->m_connection, saddr, saddr_len) < 0) {
        _DEBUG(
            "client connect failed %i", errno);
        return SRV_ERROR_CONNNECTION_ERROR;
    }

    r = write((*connection)->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    if(r != sizeof(SRV_COMMANDS))
    {
        _DEBUG(
            "client OPEN_CONNECTION write failed %i", (int)r);
        return SRV_ERROR_WRITE_ERROR;
    }

   return SRV_ERROR_SUCCESS;
}

void srv_client_close_connection(struct srv_client_connection *connection)
{
    SRV_COMMANDS command = SRV_COMMANDS_CLOSE_CONNECTION;
    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    close(connection->m_connection);
    free(connection);
}

SRV_ERROR srv_client_add_ticker(struct srv_client_connection *connection, const char *ticker, int *id)
{
    SRV_COMMANDS command = SRV_COMMANDS_ADD_TICKER;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)ticker, sizeof(char)*SRV_TICKER_LEN);

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        return err;
    }

    nelem = read(connection->m_connection, (void*)id, sizeof(int));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read ticker id failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_get_ticker(struct srv_client_connection *connection, const char *ticker, int *id)
{
    SRV_COMMANDS command = SRV_COMMANDS_GET_TICKER;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;

    _DEBUG(
        "SRV_COMMANDS_GET_TICKER ticker=%s", ticker);

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)ticker, sizeof(char)*SRV_TICKER_LEN);

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        _DEBUG(
            "failed to read");
        return err;
    }

    nelem = read(connection->m_connection, (void*)id, sizeof(int));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read ticker id failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_push_tick(struct srv_client_connection *connection, int id, void *tickdata, size_t len)
{
    SRV_COMMANDS command = SRV_COMMANDS_PUSH_TICK;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    int i;
    size_t nelem;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)&id, sizeof(int));
    write(connection->m_connection, (void*)&len, sizeof(size_t));

    i=len/SRV_BUFFER_LEN;
    nelem = write(connection->m_connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*i), len%SRV_BUFFER_LEN);
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client write tick data failed %i", (int)nelem);
        return SRV_ERROR_WRITE_ERROR;
    }
    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(err != SRV_ERROR_SUCCESS) {
        _DEBUG( \
            "READ STATUS ERROR:  err=%i", err); \
        return err;
    }
    for(; i>0; --i) {
        nelem = write(connection->m_connection, (void*)(((char*)tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
        if(nelem <= 0){ // = 0 means that it closed
            _DEBUG(
                "client write tick data failed %i", (int)nelem);
            return SRV_ERROR_WRITE_ERROR;
        }
        err = SRV_ERROR_SUCCESS;
        nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
        if(err != SRV_ERROR_SUCCESS) {
            _DEBUG( \
                "READ STATUS ERROR:  err=%i", err); \
            return err;
        }
    }

    err = SRV_ERROR_SUCCESS;
    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        return err;
    }

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_pull_tick(struct srv_client_connection *connection, int id, void **tickdata, size_t *len, struct timeval* datetime)
{
    SRV_COMMANDS command = SRV_COMMANDS_PULL_TICK;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;
    int i;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)&id, sizeof(int));

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        return err;
    }

    nelem = read(connection->m_connection, (void*)len, sizeof(size_t));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data len failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    *tickdata = (void*)malloc(*len);
    memset(*tickdata, 0, *len);

    i=(*len)/SRV_BUFFER_LEN;
    nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*i), (*len)%SRV_BUFFER_LEN);
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    err = SRV_ERROR_SUCCESS;
    nelem = write(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));

    for(; i>0; --i) {
        nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
        if(nelem <= 0){ // = 0 means that it closed
            _DEBUG(
                "client read tick data failed %i", (int)nelem);
            return SRV_ERROR_READ_ERROR;
        }

        err = SRV_ERROR_SUCCESS;
        nelem = write(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    }

    nelem = read(connection->m_connection, (void*)datetime, sizeof(struct timeval));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data datetime failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_pull_tick_by_datetime(struct srv_client_connection *connection, int id, struct timeval datetime, void **tickdata, size_t *len, struct timeval* pdatetime)
{
    SRV_COMMANDS command = SRV_COMMANDS_PULL_TICK_BY_DATETIME;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;
    int i;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)&id, sizeof(int));
    write(connection->m_connection, (void*)&datetime, sizeof(struct timeval));

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        return err;
    }
    nelem = read(connection->m_connection, (void*)len, sizeof(size_t));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data len failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    *tickdata = (void*)malloc(*len);
    memset(*tickdata, 0, *len);

    i=(*len)/SRV_BUFFER_LEN;
    nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*i), (*len)%SRV_BUFFER_LEN);
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    for(; i>0; --i) {
        nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
        if(nelem <= 0){ // = 0 means that it closed
            _DEBUG(
                "client read tick data failed %i", (int)nelem);
            return SRV_ERROR_READ_ERROR;
        }
    }

    nelem = read(connection->m_connection, (void*)pdatetime, sizeof(struct timeval));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data datetime failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_pull_tick_update(struct srv_client_connection *connection, int id, void **tickdata, size_t *len, struct timeval* pdatetime)
{
    SRV_COMMANDS command = SRV_COMMANDS_PULL_TICK_UPDATE;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;
    int i;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)&id, sizeof(int));

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS)
    {
        _DEBUG(
            "error returned %i", (int)err);
        return err;
    }
    nelem = read(connection->m_connection, (void*)len, sizeof(size_t));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data len failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    *tickdata = (void*)malloc(*len);
    memset(*tickdata, 0, *len);

    i=(*len)/SRV_BUFFER_LEN;
    nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*i), (*len)%SRV_BUFFER_LEN);
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    for(; i>0; --i) {
        nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
        if(nelem <= 0){ // = 0 means that it closed
            _DEBUG(
                "client read tick data failed %i", (int)nelem);
            return SRV_ERROR_READ_ERROR;
        }
    }

    nelem = read(connection->m_connection, (void*)pdatetime, sizeof(struct timeval));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data datetime failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_subscribe(struct srv_client_connection *connection, int id)
{
    SRV_COMMANDS command = SRV_COMMANDS_SUBSCRIBE_TO_TICKER;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)&id, sizeof(int));

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        return err;
    }

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_unsubscribe(struct srv_client_connection *connection, int id)
{
    SRV_COMMANDS command = SRV_COMMANDS_UNSUBSCRIBE_TO_TICKER;
    SRV_ERROR err = SRV_ERROR_SUCCESS;
    size_t nelem;

    write(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    write(connection->m_connection, (void*)&id, sizeof(int));

    nelem = read(connection->m_connection, (void*)&err, sizeof(SRV_ERROR));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "failed to catch return error %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    if(err != SRV_ERROR_SUCCESS) {
        return err;
    }

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_client_wait_for_tick(struct srv_client_connection *connection, void **tickdata, size_t *len, struct timeval* datetime)
{
    SRV_COMMANDS command = SRV_COMMANDS_INVALID;
    int i;

    size_t nelem = read(connection->m_connection, (void*)&command, sizeof(SRV_COMMANDS));
    if(nelem <= 0) { // = 0 means that it closed
        _DEBUG(
            "failed read notification msg %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }

    if(command != SRV_COMMANDS_NOTIFY_SUBSCRIBER) {
    	return SRV_ERROR_UNKNOWN;
    }

    nelem = read(connection->m_connection, (void*)len, sizeof(size_t));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data len failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    *tickdata = (void*)malloc(*len);
    memset(*tickdata, 0, *len);

    i=(*len)/SRV_BUFFER_LEN;
    nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*i), (*len)%SRV_BUFFER_LEN);
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }
    for(; i>0; --i) {
        nelem = read(connection->m_connection, (void*)(((char*)*tickdata)+SRV_BUFFER_LEN*(i-1)), SRV_BUFFER_LEN);
        if(nelem <= 0){ // = 0 means that it closed
            _DEBUG(
                "client read tick data failed %i", (int)nelem);
            return SRV_ERROR_READ_ERROR;
        }
    }

    nelem = read(connection->m_connection, (void*)datetime, sizeof(struct timeval));
    if(nelem <= 0){ // = 0 means that it closed
        _DEBUG(
            "client read tick data datetime failed %i", (int)nelem);
        return SRV_ERROR_READ_ERROR;
    }

    return SRV_ERROR_SUCCESS;
}
