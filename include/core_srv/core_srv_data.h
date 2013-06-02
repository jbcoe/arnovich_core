/**
  * @file core_srv_data.h
  * @brief The \c srv_tick data type and related functions.
  */

#ifndef SRV_DATA_H_
#define SRV_DATA_H_

#include <pthread.h>
#include <sys/time.h>

#include <core_srv/core_srv_types.h>

struct srv_ticker_info
{
    char m_ticker[SRV_TICKER_LEN];
};

struct srv_tick
{
    struct timeval    m_datetime;
    void             *m_data;
    size_t            m_len;
    struct srv_tick*  m_next;
};

struct srv_subscriber
{
    int m_connection;
    struct srv_subscriber* m_next;
};

struct srv_ticker
{
    struct srv_tick         *m_ticks;
    pthread_mutex_t          m_tickslock;
    struct srv_ticker_info   m_info;
    pthread_mutex_t          m_subscriblock;
    struct srv_subscriber*   m_subscribers;
};


struct srv_tickers
{
    struct srv_ticker**      m_tickers;
    pthread_mutex_t          m_tickerslock;
    int                      m_ntickers;
};

void srv_data_init_tick(struct srv_tick **tick, void* data, size_t len);

void srv_data_free_tick(struct srv_tick *tickers);

void srv_data_init_tickers(struct srv_tickers **tickers);

void srv_data_free_tickers(struct srv_tickers *tickers);

SRV_ERROR srv_add_ticker(struct srv_tickers *tickers, char *ticker, int *id);

SRV_ERROR srv_get_ticker(struct srv_tickers *tickers, char *ticker, int *id);

SRV_ERROR srv_push_tick(struct srv_tickers *tickers, int id, void *tickdata, size_t len);

SRV_ERROR srv_pull_tick(struct srv_tickers *tickers, int id, void **tickdata, size_t *len, struct timeval *datetime);

SRV_ERROR srv_pull_tick_by_datetime(struct srv_tickers *tickers, int id, struct timeval datetime, void **tickdata, size_t *len, struct timeval *pdatetime);

SRV_ERROR srv_pull_tick_update(struct srv_tickers *tickers, int id, struct timeval datetime, void **tickdata, size_t *len, struct timeval *pdatetime);

SRV_ERROR srv_tick_subscribe(struct srv_tickers *tickers, int id, int connection);

SRV_ERROR srv_tick_unsubscribe(struct srv_tickers *tickers, int id, int connection);

#endif /* SRV_DATA_H_ */
