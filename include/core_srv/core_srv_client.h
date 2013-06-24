
/**
  * @file core_srv_client.h
  * @brief The client side functionality
  */

#ifndef SRV_CLIENT_H_
#define SRV_CLIENT_H_

#include <core_srv/core_srv_types.h>


#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <time.h>

struct srv_client_connection
{
    int m_connection;
};

SRV_ERROR srv_client_connect(struct srv_client_connection **, SRV_SOCKET_TYPES socket_type);

void srv_client_close_connection(struct srv_client_connection *connection);

SRV_ERROR srv_client_add_ticker(struct srv_client_connection *connection, const char *ticker, int *id);

SRV_ERROR srv_client_get_ticker(struct srv_client_connection *connection, const char *ticker, int *id);

SRV_ERROR srv_client_push_tick(struct srv_client_connection *connection, int id, void *tickdata, size_t len);

SRV_ERROR srv_client_pull_tick(struct srv_client_connection *connection, int id, void **tickdata, size_t *len, struct timeval* datetime);

SRV_ERROR srv_client_pull_tick_by_datetime(struct srv_client_connection *connection, int id, struct timeval datetime, void **tickdata, size_t *len, struct timeval* pdatetime);

SRV_ERROR srv_client_pull_tick_update(struct srv_client_connection *connection, int id, void **tickdata, size_t *len, struct timeval* pdatetime);

SRV_ERROR srv_client_subscribe(struct srv_client_connection *connection, int id);

SRV_ERROR srv_client_unsubscribe(struct srv_client_connection *connection, int id);

SRV_ERROR srv_client_wait_for_tick(struct srv_client_connection *connection, void **tickdata, size_t *len, struct timeval* datetime);

#endif /* SRV_CLIENT_H_ */
