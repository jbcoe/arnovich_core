
#include <core_srv/core_srv_server.h>

#include <stdlib.h>
#include <stdio.h>

int main()
{
    //SRV_ERROR rtn = srv_run_server(SRV_SOCKET_TYPES_TCP_IP);
    SRV_ERROR rtn = srv_run_server(SRV_SOCKET_TYPES_LOCAL_PIPES);
	if(rtn == SRV_ERROR_SUCCESS)
	{
		exit(1);
	}
    exit(0);
}
