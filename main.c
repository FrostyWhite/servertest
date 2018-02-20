//
//  main.cpp
//  networkcomms
//
//  Created by Kuura Parkkola on 18/02/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

typedef struct{
	char *fields;
	char **values;
} data_t;

typedef struct{
	data_t data;
	char tag[20];
	char html_f[10];
	unsigned int cnt;
	unsigned int left;
	char* (*parseFunc)(void *);
} tag;

typedef char* (*parseFunc)(tag);

char *parseTag(void *p_task){	// The basic parse function
	tag *task = (tag *) p_task;
	char *data = (task->data.fields != NULL) ? task->data.fields : task->data.values[task->cnt - task->left];
	char *parsed = calloc((2 * strlen(task->html_f) + strlen(data) + 7), sizeof(char));
	sprintf(parsed, "<%s>%s</%s>", task->html_f, data, task->html_f);
	if(task->data.fields != NULL){
		free(task->data.fields);
		task->data.fields = NULL;
	}
	return parsed;
}

char *render_html(char *path, ...){
	const unsigned int buffersize = 2048;
	
	va_list templatetags;
	va_start(templatetags, path);
	FILE *fp = fopen(path, "r");
	
	char *render = calloc(buffersize, sizeof(char));
	unsigned int left = buffersize;
	
	tag processed = va_arg(templatetags, tag);
	char line[256];
	while (fgets(line, 255, fp) != NULL) {
		char *loc = 0;
		if ((loc = strstr(line, processed.tag))/* && processed.data != NULL*/){
			while(processed.left > 0){
				int index = (int) (loc - line);
				char *tagdata = processed.parseFunc(&processed);
				int linelen = (int) (strlen(line) - strlen(processed.tag) + strlen(tagdata));
				while(left < linelen){
					render = realloc(render, sizeof(render) + buffersize * sizeof(char));
					left += buffersize;
				}
				strncat(render, line, index);
				strcat(render, tagdata);
				strcat(render, (loc + strlen(tagdata)));
				strcat(render, "\n");
				free(processed.data.values[processed.cnt - processed.left]);
				processed.left--;
			}
			free(processed.data.values);
			processed = va_arg(templatetags, tag);
		}
		else {
			while(left < strlen(line)){
				render = realloc(render, sizeof(render) + buffersize * sizeof(char));
				left += buffersize;
			}
			strcat(render, line);
		}
		memset(line, 0, 256);
	}
	return render;
}

char *handle_HTTP(char *request){
	if(strstr(request, "GET") == request){
		
	} else if (strstr(request, "POST") == request){
		return "HTTP/1.1 501 Not Implemented\r\n\r\n";
	}
	return NULL;
}

int main(int argc, char *argv[]){
	
	// TEST DATA //
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
	
	tag end;
	end.parseFunc = NULL;
	end.cnt = 0;
	end.left = 0;
	end.data.fields = NULL;
	end.data.values = NULL;
	
	//printf("%s", render_html("./files/index.html", num1, num2, num3, end));
	
	// TEST DATA ENDS //
	
	// REQ CTRL //
	char header[2048] = "HTTP/1.1 200 OK\r\n\r\n";
	
	int localsocket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8001);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	
	bind(localsocket, (struct sockaddr *) &local_addr, sizeof(local_addr));
	listen(localsocket, 15);
	
	int clientat;
	char ret[4096];
	char *response;
	char httpresponse[4096];
	
	while(1){
		clientat = accept(localsocket, NULL, NULL);
		recv(clientat, &ret, sizeof(ret), 0);
		response = render_html("./files/index.html", num1, num2, num3, end);
		sprintf(httpresponse, "%s%s\r\n\r\n", header, response);
		printf("%s\n\n", httpresponse);
		send(clientat, httpresponse, sizeof(httpresponse), 0);
		close(clientat);
	}
	
	// REQ CTRL ENDS //
	return 0;
}
