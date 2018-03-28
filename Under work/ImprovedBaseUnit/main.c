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

void *clithread(void *args){
	int clientsock = *((int *) args);
	pthread_detach(pthread_self());
	
	printf("\nClient being served\n");
	
	char readbuffer[2048] = {0};
	ssize_t n;
	
	while ((n = read(clientsock, readbuffer, 2047)) > 0) {
		printf("\n\n\tFrom client:\n");
		char body[] = "<!DOCTYPE HTML>\n<HTML>\n\t<HEAD>\n\t\t<title>HELLO</title>\n\t</head>\n\t<body>\n\t\t<p>HELLO</p>\n\t</body>\n</html>";
		char header[] = "HTTP/1.1 200 OK\nContent-Type: text/html\nServer: Icicle a1.0";
		char response[2048] = {0};
		sprintf(response, "%s\nContent-Length: %d\n\n%s\n\n", header, (int) strlen(body), body);
		write(STDOUT_FILENO, readbuffer, n);
		write(clientsock, response, strlen(response));
		memset(readbuffer, 0, 2048);
		
	}
	
	printf("\nClient has left\n");
	
	close(clientsock);
	pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
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
	servaddr.sin6_port = htons(8002);
	
	if (bind(listenfd, (struct sockaddr *) &servaddr,
			 sizeof(servaddr)) < 0) {
		perror("bind");
		return -1;
	}
	
	if (listen(listenfd, 5) < 0) {
		perror("listen");
		return -1;
	}
	
	for ( ; ; ) {
		len = sizeof(cliaddr);
		
		printf("WAITING\n");
		
		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr,
							 &len)) < 0) {
			perror("accept");
			return -1;
		}
		printf("Client from %s, port %d entered\n",
			   inet_ntop(AF_INET6, &cliaddr.sin6_addr,
						 buff, sizeof(buff)), ntohs(cliaddr.sin6_port));
		
		//kommunikoi connfd-pistokkeen kautta täällä tai luo sille uusi säie
		pthread_t newtid;
		pthread_create(&newtid, NULL, clithread, &connfd);
	}
}
