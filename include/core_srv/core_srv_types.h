/*
 * @file srv_types.h
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
