//
//  main.cpp
//  networkcomms
//
//  Created by Kuura Parkkola on 18/02/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

// C standard library dependencies
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "render.h"
#include "HTTPReq.h"

int main_old(int argc, char *argv[]){
	
	// REQ CTRL //
	
	// Creating TCP socket
	int localsocket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Creating the local address
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8000);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	
	// Binding the socket and the address
	bind(localsocket, (struct sockaddr *) &local_addr, sizeof(local_addr));
	
	// Opening the TCP socket in the local address
	listen(localsocket, 15);
	
	// Creating resources for accepting client requests
	// and responding to them
	int clientat;
	char ret[4096];
	memset(ret, 0, 4096);
	
	// Server runloop
	while(1){
		memset(ret, 0, 4096);
		
		// blocks until client sends request
		clientat = accept(localsocket, NULL, NULL);
		
		// receives client first request
		recv(clientat, &ret, sizeof(ret), 0);
		
		char *response = handleRequest(ret);
		
		//Debug print
		printf("Request:\n%s\nResponse:\n\n%s", ret, response);
		
		// Sends the response
		send(clientat, response, sizeof(response), 0);
		
		// Closes the connection to give space for other
		// connections as only one is accepted at a given time
		close(clientat);
	}
	
	// REQ CTRL ENDS //
	
	// Program never reaches this point
	// and is closed via a signal
	return 0;
}
