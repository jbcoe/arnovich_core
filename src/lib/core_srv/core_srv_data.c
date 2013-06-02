
/**
  * @file core_srv_data.c
  * @brief Implementation file for data type.
  */

#include <core_srv/core_srv_data.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void srv_data_init_tick(struct srv_tick **tick, void* data, size_t len)
{
    (*tick) = (struct srv_tick*)malloc(sizeof(struct srv_tick));
    (*tick)->m_data = (void*)malloc(len);
    memcpy((*tick)->m_data, data, len);
    (*tick)->m_len = len;
    gettimeofday(&((*tick)->m_datetime), NULL);
}

void srv_data_free_tick(struct srv_tick *tick)
{
    free(tick->m_data);
    srv_data_free_tick(tick->m_next);
    free(tick);
}

void srv_data_init_tickers(struct srv_tickers **tickers)
{
    (*tickers) = (struct srv_tickers *)malloc(sizeof(struct srv_tickers ));
    (*tickers)->m_ntickers = 0;
    (*tickers)->m_tickers = NULL;
    pthread_mutex_init(&(*tickers)->m_tickerslock, NULL);
}

void srv_data_free_tickers(struct srv_tickers *tickers)
{
    pthread_mutex_destroy(&tickers->m_tickerslock);
    free(tickers->m_tickers);
    int i;
    for(i=0;i<tickers->m_ntickers;++i)
    {
        pthread_mutex_destroy(&tickers->m_tickers[i]->m_tickslock);
        pthread_mutex_destroy(&tickers->m_tickers[i]->m_subscriblock);
        srv_data_free_tick(tickers->m_tickers[i]->m_ticks);
    }
    free(tickers);
}

SRV_ERROR srv_get_ticker(struct srv_tickers *tickers, char *ticker, int *id)
{
    int i;
    pthread_mutex_lock(&tickers->m_tickerslock);
    for(i=0;i<tickers->m_ntickers;++i)
    {
        if(strcmp(ticker, tickers->m_tickers[i]->m_info.m_ticker) == 0)
        {
            *id = i;
            pthread_mutex_unlock(&tickers->m_tickerslock);
            return SRV_ERROR_SUCCESS;
        }
    }
    pthread_mutex_unlock(&tickers->m_tickerslock);
    return SRV_ERROR_UNKNOWN;
}

SRV_ERROR srv_add_ticker(struct srv_tickers *tickers, char *ticker, int *id)
{
    //TODO check that the ticker doesn't already exist?
    struct srv_ticker** tmp;

    pthread_mutex_lock(&tickers->m_tickerslock);
    tmp = tickers->m_tickers;
    tickers->m_tickers = (struct srv_ticker**)malloc(sizeof(struct srv_ticker*)*(tickers->m_ntickers+1));
    memcpy(tickers->m_tickers, tmp, sizeof(struct srv_ticker*)*tickers->m_ntickers);
    free(tmp);

    tickers->m_tickers[tickers->m_ntickers] = (struct srv_ticker*)malloc(sizeof(struct srv_ticker));
    tickers->m_tickers[tickers->m_ntickers]->m_ticks = NULL;
    tickers->m_tickers[tickers->m_ntickers]->m_subscribers = NULL;
    pthread_mutex_init(&tickers->m_tickers[tickers->m_ntickers]->m_tickslock, NULL);
    pthread_mutex_init(&tickers->m_tickers[tickers->m_ntickers]->m_subscriblock, NULL);
    strcpy(tickers->m_tickers[tickers->m_ntickers]->m_info.m_ticker, ticker);
    ++tickers->m_ntickers;

    *id = tickers->m_ntickers - 1;
    pthread_mutex_unlock(&tickers->m_tickerslock);

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_push_tick(struct srv_tickers *tickers, int id, void *tickdata, size_t len)
{
    if(id >= tickers->m_ntickers) {
        return SRV_ERROR_UNKNOWN_TICKER;
    }

    struct srv_tick *new_tick;
    srv_data_init_tick(&new_tick, tickdata, len);

    pthread_mutex_lock(&tickers->m_tickers[id]->m_tickslock);
    new_tick->m_next = tickers->m_tickers[id]->m_ticks;
    tickers->m_tickers[id]->m_ticks = new_tick;
    pthread_mutex_unlock(&tickers->m_tickers[id]->m_tickslock);
    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_pull_tick(struct srv_tickers *tickers, int id, void **tickdata, size_t *len, struct timeval *datetime)
{
    if(id >= tickers->m_ntickers)
    {
        return SRV_ERROR_UNKNOWN_TICKER;
    }
    if(tickers->m_tickers[id]->m_ticks == NULL)
    {
    	return SRV_ERROR_NO_TICKDATA;
    }

    pthread_mutex_lock(&tickers->m_tickers[id]->m_tickslock);
    struct srv_tick *tick = tickers->m_tickers[id]->m_ticks;
    pthread_mutex_unlock(&tickers->m_tickers[id]->m_tickslock);

    *tickdata = (void*)malloc(tick->m_len);
    memcpy(*tickdata, tick->m_data, tick->m_len);
    *len = tick->m_len;
    *datetime = tick->m_datetime;

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_pull_tick_by_datetime(struct srv_tickers     *tickers,
                                  int                    id,
                                  struct timeval         datetime,
                                  void                 **tickdata,
                                  size_t                *len,
                                  struct timeval        *pdatetime)
{
    if(id >= tickers->m_ntickers)
    {
        return SRV_ERROR_UNKNOWN_TICKER;
    }
    if(tickers->m_tickers[id]->m_ticks == NULL)
    {
    	return SRV_ERROR_NO_TICKDATA;
    }

    pthread_mutex_lock(&tickers->m_tickers[id]->m_tickslock);
    struct srv_tick *tick = tickers->m_tickers[id]->m_ticks;
    pthread_mutex_unlock(&tickers->m_tickers[id]->m_tickslock);

    while((tick != NULL) &&
          ((tick->m_datetime.tv_sec > datetime.tv_sec) ||
          ((tick->m_datetime.tv_sec == datetime.tv_sec) &&
           (tick->m_datetime.tv_usec > datetime.tv_usec))))
    {
    	tick = tick->m_next;
    }

    if(tick == NULL)
    {
    	return SRV_ERROR_NO_TICKDATA;
    }

    *tickdata = (void*)malloc(tick->m_len);
    memcpy(*tickdata, tick->m_data, tick->m_len);
    *len = tick->m_len;
    *pdatetime = tick->m_datetime;

    return SRV_ERROR_SUCCESS;
}


SRV_ERROR srv_pull_tick_update(struct srv_tickers     *tickers,
                             int                    id,
                             struct timeval         datetime,
                             void                 **tickdata,
                             size_t                *len,
                             struct timeval        *pdatetime)
{
    if(id >= tickers->m_ntickers)
    {
        return SRV_ERROR_UNKNOWN_TICKER;
    }
    if(tickers->m_tickers[id]->m_ticks == NULL)
    {
        return SRV_ERROR_NO_TICKDATA;
    }

    pthread_mutex_lock(&tickers->m_tickers[id]->m_tickslock);
    struct srv_tick *tick = tickers->m_tickers[id]->m_ticks;
    pthread_mutex_unlock(&tickers->m_tickers[id]->m_tickslock);

    if((tick->m_datetime.tv_sec < datetime.tv_sec) ||
       ((tick->m_datetime.tv_sec == datetime.tv_sec) &&
        (tick->m_datetime.tv_usec < datetime.tv_usec)))
    {
        return SRV_ERROR_NO_TICKDATA;
    }

    while((tick->m_next != NULL) &&
        ((tick->m_next->m_datetime.tv_sec > datetime.tv_sec) ||
        ((tick->m_next->m_datetime.tv_sec == datetime.tv_sec) &&
         (tick->m_next->m_datetime.tv_usec > datetime.tv_usec))))
    {
        tick = tick->m_next;
    }

    if(tick == NULL)
    {
        return SRV_ERROR_NO_TICKDATA;
    }

    *tickdata = (void*)malloc(tick->m_len);
    memcpy(*tickdata, tick->m_data, tick->m_len);
    *len = tick->m_len;
    *pdatetime = tick->m_datetime;

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_tick_subscribe(struct srv_tickers *tickers, int id, int connection)
{
    if(id >= tickers->m_ntickers)
    {
        return SRV_ERROR_UNKNOWN_TICKER;
    }

    struct srv_subscriber* subscr;
    subscr = (struct srv_subscriber*)malloc(sizeof(struct srv_subscriber));
    subscr->m_connection = connection;
    subscr->m_next = NULL;

    pthread_mutex_lock(&tickers->m_tickers[id]->m_tickslock);
    subscr->m_next = tickers->m_tickers[id]->m_subscribers;
    tickers->m_tickers[id]->m_subscribers = subscr;
    pthread_mutex_unlock(&tickers->m_tickers[id]->m_tickslock);

    return SRV_ERROR_SUCCESS;
}

SRV_ERROR srv_tick_unsubscribe(struct srv_tickers *tickers, int id, int connection)
{
    if(id >= tickers->m_ntickers)
    {
        return SRV_ERROR_UNKNOWN_TICKER;
    }

    pthread_mutex_lock(&tickers->m_tickers[id]->m_tickslock);

    struct srv_subscriber *subscr, *subscr_prev;
    subscr = tickers->m_tickers[id]->m_subscribers;
    subscr_prev = NULL;

    while(subscr != NULL) {
    	if(subscr->m_connection  == connection) {
    		break;
    	}
    	subscr_prev = subscr;
    	subscr = subscr->m_next;
    }

    if(subscr != NULL) {
    	if(subscr_prev == NULL) {
    		tickers->m_tickers[id]->m_subscribers = subscr->m_next;
    	} else {
    		subscr_prev->m_next = subscr->m_next;
    	}
    	free(subscr);
    }

    pthread_mutex_unlock(&tickers->m_tickers[id]->m_tickslock);

    return SRV_ERROR_SUCCESS;
}
