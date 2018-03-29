//
//  main.c
//  ImprovedBaseUnit
//
//  Created by Kuura Parkkola on 27/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "render.h"

int run = 1;
int active = 0;

void *clithread(void *args){
	int clientsock = *((int *) args);
	pthread_detach(pthread_self());
	
	active++;
	
	printf("\nClient being served\n");
	
	char request[2048] = {0};
	char *response = NULL;
	ssize_t n;
	
	while ((n = read(clientsock, request, 2047)) > 0) {
		
		response = handleRequest(request);
		printf("\n\n\tFrom client:\n");
		//char body[] = "<!DOCTYPE HTML>\n<HTML>\n\t<HEAD>\n\t\t<title>Tespage</title>\n\t</head>\n\t<body>\n\t\t<h1>Hello world!!</h1>\n\t\t<p>I am very pleased to meet you!</p>\n\t</body>\n</html>";
		//char header[] = "HTTP/1.1 200 OK\nContent-Type: text/html\nServer: Icicle a1.0\nConnection: Close";
		//char response[2048] = {0};
		//sprintf(response, "%s\nContent-Length: %d\n\n%s\n\n", header, (int) strlen(body), body);
		write(STDOUT_FILENO, request, n);
		
		printf("\n\n\tTo client:\n");
		write(STDOUT_FILENO, response, strlen(response));
		
		write(clientsock, response, strlen(response));
		free(response);
		memset(request, 0, 2048);
		
	}
	
	printf("\nClient has left\n");
	
	close(clientsock);
	active--;
	
	pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
	
	pthread_t newtid;
	
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in6 servaddr, cliaddr;
	char buff[80];
	
	if ((listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr = in6addr_any;
	servaddr.sin6_port = htons(8889);
	
	if (bind(listenfd, (struct sockaddr *) &servaddr,
			 sizeof(servaddr)) < 0) {
		perror("bind");
		return -1;
	}
	
	if (listen(listenfd, 5) < 0) {
		perror("listen");
		return -1;
	}
	
	printf("Run: %d\n", run);
	
	while((connfd = accept(listenfd, (struct sockaddr *) &cliaddr,
						   &len)) >= 0) {
		len = sizeof(cliaddr);
		
		printf("Client from %s, port %d entered\n",
			   inet_ntop(AF_INET6, &cliaddr.sin6_addr,
						 buff, sizeof(buff)), ntohs(cliaddr.sin6_port));
		
		pthread_create(&newtid, NULL, clithread, &connfd);
		
		printf("Active clients: %d\n", active);
		
		printf("Run: %d\n", run);
	}
	
	close(listenfd);
	
	while(active){
		sleep(5);
	}
	
	return 0;
}
