
/**
 * @package core_srv
 * @brief A simple push-pop-notiication server with a C and Python interface.
 *
 * This module provides a very simple server component.
 * 
 * The server can be started by calling srv_run_server or using core_srv_run.c.
 * 
 * Once started you can create tickers and push data to tickers using either the C-interface(core_srv_client.h) or the Python interface(core_srv.h).
 *
 * Data can be fetched either by subscription, time of data or simply as the most recent.
 *
 * <b>Example</b>
 *
 * A few simple Python examples of how to create a ticker and push data.
 * \code
 * import arnovich.core as core
 * connection = core.connection()
 * ticker_id = connection.add_ticker("MY_TICKER")
 * # a ticker is pushed by using the ticker_id
 * connection.push_ticker(ticker_id, "MY DATA TO BE PUSHED")
 * # you can get the ticker by doing the following
 * ticker_id = connection.get_ticker("MY_TICKER")
 * \endcode
 *
 * A simple example of how to fetch data from a ticker.
 * 
 * \code
 * import arnovich.core as core
 * connection = core.connection()
 * ticker_id = connection.get_ticker("MY_TICKER")
 * # pulls the most recent data for the ticker
 * data = connection.pull_tick(ticker_id)
 * # only pulls if a new update has happened since last read
 * data = connection.pull_tick(ticker_id)
 * \endcode
 *
 * A simple example of how to subscribe to a ticker.
 *
 * \code
 * import arnovich.core as core
 * connection = core.connection()
 * ticker_id = connection.get_ticker("MY_TICKER")
 * connection.subscribe(ticker_id)
 * while True:
 *   # this will wait for an update to happen before continuing
 *   data = connection.wait_for_update(ticker_id)
 * \endcode
 *
 * <b>Files of interest</b>
 * - core_srv/core_srv_types.h
 * - core_srv/core_srv_data.h
 * - core_srv/core_srv_client.h
 * - core_srv/core_srv_server.h
 * - bin/core_srv_run.c
 */


/**
  * @file core_srv_types.h
  * @brief Shared core_srv types.
  */

#ifndef SRV_TYPES_H_
#define SRV_TYPES_H_

typedef enum {
    SRV_ERROR_SUCCESS               = 0,
    SRV_ERROR_UNKNOWN               = 1,
    SRV_ERROR_CONNNECTION_ERROR     = 2,
    SRV_ERROR_WRITE_ERROR           = 3,
    SRV_ERROR_READ_ERROR            = 4,
    SRV_ERROR_UNKNOWN_TICKER        = 5,
    SRV_ERROR_NO_TICKDATA           = 6,
    SRV_ERROR_TICKER_EXISTS         = 7
} SRV_ERROR;

#define SRV_TICKER_LEN        20

//#define SRV_BUFFER_LEN        16000
//OSX seems to only support 8192
#define SRV_BUFFER_LEN        8000

typedef enum {
    SRV_COMMANDS_INVALID                           = -1,
    SRV_COMMANDS_OPEN_CONNECTION                   = 0,
    SRV_COMMANDS_CLOSE_CONNECTION                  = 1,
    SRV_COMMANDS_ADD_TICKER                        = 2,
    SRV_COMMANDS_GET_TICKER                        = 3,
    SRV_COMMANDS_PUSH_TICK                         = 4,
    SRV_COMMANDS_PULL_TICK                         = 5,
    SRV_COMMANDS_PULL_TICK_BY_DATETIME             = 6,
    SRV_COMMANDS_PULL_TICK_UPDATE                  = 7,
    SRV_COMMANDS_SUBSCRIBE_TO_TICKER               = 8,
    SRV_COMMANDS_UNSUBSCRIBE_TO_TICKER             = 9,
    SRV_COMMANDS_NOTIFY_SUBSCRIBER                 = 10
} SRV_COMMANDS;

typedef enum {
	SRV_SOCKET_TYPES_LOCAL_PIPES                   = 0,
    SRV_SOCKET_TYPES_TCP_IP                        = 1
} SRV_SOCKET_TYPES;

#define SRV_LOCAL_PIPES_SERVER_ADDRESS             "/tmp/server"

#define SRV_TCP_IP_SERVER_ADDRESS                  "127.0.0.1" //localhost
#define SRV_TCP_IP_SERVER_PORT                     1025

#endif /* SRV_TYPES_H_ */
