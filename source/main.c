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

// Implementation in progress
char *handle_HTTP(char *request){
	if(strstr(request, "GET") == request){
		
	} else if (strstr(request, "POST") == request){
		return "HTTP/1.1 501 Not Implemented\r\n\r\n";
	}
	return NULL;
}

int main(int argc, char *argv[]){
	
	// REQ CTRL //
	
	// responce for valid HTTP request
	char header[2048] = "HTTP/1.1 200 OK\r\n\r\n";
	
	// Creating TCP socket
	int localsocket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Creating the local address
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8001);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	
	// Binding the socket and the address
	bind(localsocket, (struct sockaddr *) &local_addr, sizeof(local_addr));
	
	// Opening the TCP socket in the local address
	listen(localsocket, 15);
	
	// Creating resources for accepting client requests
	// and responding to them
	int clientat;
	char ret[4096];
	char *response;
	char httpresponse[4096];
	memset(httpresponse, 0, 4096);
	// ^^^ MEMSET THIS!!
	
	// Server runloop
	while(1){
		
		// TEST DATA //
		tag_t title = dataFromVA("{%%TITLE%%}", "title", "%1v", NULL, "Actorpage", NULL);
		
		tag_t head = dataFromVA("{%%HEADER%%}", "h1", "%1v", NULL, "Cool actors", NULL);
		
		tag_t render = dataFromFile("{%%CONTENT%%}", "p", "%-15a %-+20a %-20a", "./database/characters");
		
		// A debug print
		//printf("%s", render_html("./files/index.html", num1, num2, render, end));
		
		// TEST DATA ENDS //
		
		// blocks until client sends request
		clientat = accept(localsocket, NULL, NULL);
		
		// receives client first request
		recv(clientat, &ret, sizeof(ret), 0);
		
		printf("%s", ret);
		
		// renders a responce html document
		response = render_html("./files/index.html", title, head, render, makeEnd());
		
		// forms the actual http responce
		sprintf(httpresponse, "%s%s\r\n\r\n", header, response);
		
		//Debug print
		//printf("%s\n\n", httpresponse);
		
		// Sends the response
		send(clientat, httpresponse, sizeof(httpresponse), 0);
		
		// Closes the connection to give space for other
		// connections as only one is accepted at a given time
		close(clientat);
	}
	
	// REQ CTRL ENDS //
	
	// Program never reaches this point
	// and is closed via a signal
	return 0;
}
