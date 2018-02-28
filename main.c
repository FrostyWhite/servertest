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

// Required tools and typedefinitions //

typedef struct{
	char *fields; // fromatted string containing the data fields
	char **values; // formatted datastrings
} data_t;

typedef struct{
	data_t data; // Data to be rendered
	char tag[20]; // Tag to be replaced in html template
	char html_f[10]; // html tags to be added to the data for rendering
	unsigned int cnt; // how many datastrings contained in package
	unsigned int left; // how many datastrings left unprocessed
	char* (*parseFunc)(void *); // function used for rendering datastrings
} tag;

// typedefinition for easier utilization of function pointer
typedef char* (*parseFunc)(tag);

// Takes taglines from given data and forms
// html tagged lines out of them
// *** TODO: FIX left update issue here and in render_html ***
char *parseTag(void *p_task){	// The basic parse function
	
	// typecast datapacket
	tag *task = (tag *) p_task;
	
	// Determine whether a field specification header is required
	char *data = (task->data.fields != NULL) ? task->data.fields : task->data.values[task->cnt - task->left];
	
	// Allocate memory for the html line and form the line
	char *parsed = calloc((2 * strlen(task->html_f) + strlen(data) + 7), sizeof(char));
	sprintf(parsed, "<%s>%s</%s>", task->html_f, data, task->html_f);
	
	// If there was a field specification line it is now
	// processed and is NULLed so that it won't get printed again
	if(task->data.fields != NULL){
		free(task->data.fields);
		task->data.fields = NULL;
	}
	return parsed;
}

// Renders the html document at 'path'
// filling it with the given data

char *render_html(char *path, ...){
	// block length of memoryallocations
	const unsigned int buffersize = 2048;
	
	// setting up the variable argument list
	va_list templatetags;
	va_start(templatetags, path);
	
	// setting up resources
	FILE *fp = fopen(path, "r");
	char *render = calloc(buffersize, sizeof(char));
	unsigned int left = buffersize;
	
	// picking the first va argument
	tag processed = va_arg(templatetags, tag);
	
	char line[256]; // Lines can't realistically be this long
	
	// Reads the template line by line
	while (fgets(line, 255, fp) != NULL) {
		char *loc = 0;
		
		// if the tag given in 'processed' is found
		if ((loc = strstr(line, processed.tag))){
			
			// processes as many text blocks as there are in the
			// 'processed' datablock
			while(processed.left > 0){
				
				// Calculates some stuff with indices
				int index = (int) (loc - line);
				char *tagdata = processed.parseFunc(&processed);
				int linelen = (int) (strlen(line) - strlen(processed.tag) + strlen(tagdata));
				
				// Allocates more data if necessary
				while(left < linelen){
					render = realloc(render, sizeof(render) + buffersize * sizeof(char));
					left += buffersize;
				}
				
				// writes the data into the render
				strncat(render, line, index);
				strcat(render, tagdata);
				strcat(render, (loc + strlen(tagdata)));
				strcat(render, "\n");
				
				// cleans up data that is no longer needed
				free(processed.data.values[processed.cnt - processed.left]);
				processed.left--;
			}
			
			// 'processed' datablock is now processed
			// so it is cleaned and a new one is loaded
			if (processed.cnt > 0) {
				free(processed.data.values);
				processed = va_arg(templatetags, tag);
			}
		}
		
		// if line contains no tags
		else {
			
			// more memory is allocated if necessary
			while(left < strlen(line)){
				render = realloc(render, sizeof(render) + buffersize * sizeof(char));
				left += buffersize;
			}
			
			// the line is written into render
			strcat(render, line);
		}
		
		// the line gets reset for reading another line
		memset(line, 0, 256);
	}
	
	// the finished render is returned as valid html document
	fclose(fp);
	return render;
}


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
		// creation of some data packets for rendering
		char **data1 = calloc(1, sizeof(char*));
		char *content1 = calloc(10, sizeof(char));
		strcpy(content1, "Titlehere");
		data1[0] = content1;
		tag num1;
		num1.parseFunc = parseTag;
		num1.data.fields = NULL;
		num1.data.values = data1;
		num1.cnt = 1;
		num1.left = 1;
		strcpy(num1.tag, "{%%TITLE%%}");
		strcpy(num1.html_f, "title");
		
		char **data2 = calloc(1, sizeof(char*));
		char *content2 = calloc(10, sizeof(char));
		strcpy(content2, "Some Head");
		data2[0] = content2;
		tag num2;
		num2.parseFunc = parseTag;
		num2.data.fields = NULL;
		num2.data.values = data2;
		num2.cnt = 1;
		num2.left = 1;
		strcpy(num2.tag, "{%%HEADER%%}");
		strcpy(num2.html_f, "h1");
		
		char **data3 = calloc(4, sizeof(char*));
		char *content3_1 = calloc(10, sizeof(char));
		strcpy(content3_1, "autogen 1");
		char *content3_2 = calloc(10, sizeof(char));
		strcpy(content3_2, "autogen 2");
		char *content3_3 = calloc(10, sizeof(char));
		strcpy(content3_3, "autogen 3");
		char *content3_4 = calloc(10, sizeof(char));
		strcpy(content3_4, "autogen 4");
		data3[0] = content3_1;
		data3[1] = content3_2;
		data3[2] = content3_3;
		data3[3] = content3_4;
		tag num3;
		num3.parseFunc = parseTag;
		num3.data.fields = NULL;
		num3.data.values = data3;
		num3.cnt = 4;
		num3.left = 4;
		strcpy(num3.tag, "{%%CONTENT%%}");
		strcpy(num3.html_f, "p");
		
		// Ending packet so rendering function knows when to stop
		tag end;
		end.parseFunc = NULL;
		end.cnt = 0;
		end.left = 0;
		end.data.fields = NULL;
		end.data.values = NULL;
		
		// A debug print
		//printf("%s", render_html("./files/index.html", num1, num2, num3, end));
		
		// TEST DATA ENDS //
		
		// blocks until client sends request
		clientat = accept(localsocket, NULL, NULL);
		
		// receives client first request
		recv(clientat, &ret, sizeof(ret), 0);
		
		printf("%s", ret);
		
		// renders a responce html document
		response = render_html("./files/index.html", num1, num2, num3, end);
		
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
