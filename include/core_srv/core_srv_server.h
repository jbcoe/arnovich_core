
/**
  * @file core_srv_server.h
  * @brief The server side functionality
  */

#ifndef SRV_SERVER_H_
#define SRV_SERVER_H_

#include <core_srv/core_srv_types.h>

/**
 * @brief Start server using \c socket_type.
 */
SRV_ERROR srv_run_server(SRV_SOCKET_TYPES socket_type);

#endif /* SRV_SERVER_H_ */
