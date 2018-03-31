//
//  request.c
//  Version2Unit
//
//  Created by Kuura Parkkola on 31/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include "request.h"

char *processRequest(char *request){
	req_t *req_data = getReqAttribs(request);
	rsc_t pageinfo = getPage(req_data->resource);
	return pageinfo.viewFunc(pageinfo.filepath, pageinfo.datapath, req_data);
}

char *generateResponse(char *content, char *status){
	char *response = calloc(512 + strlen(content), sizeof(char));
	switch (atoi(status)) {
		case 200:
			sprintf(response, "HTTP/1.1 200 OK\nServer: Icicle Server\nConnection: keep-alive\nContent-Type: text/html; charset=UTF-8\nContent-Length: %d\n\n%s\n\n", (int) strlen(content), content);
			free(content);
			return response;
			break;
		case 404:
			sprintf(response, "HTTP/1.1 404 Not Found\nContent-Length: %d\n\n%s\n\n", (int) strlen(content), content);
			free(content);
			return response;
			break;
		default:
			return NULL;
			break;
	}
	return NULL;
}

req_t *getReqAttribs(char *request){
	req_t *target = calloc(1, sizeof(req_t));
	target->additionals = NULL;
	char *ptr = request;
	int line = 0; int fieldi = 0;
	while(*ptr != ' '){
		target->method[fieldi] = *ptr;
		ptr++; fieldi++;
	}
	target->method[fieldi] = 0;
	ptr++; fieldi = 0;
	while(*ptr != ' '){
		target->resource[fieldi] = *ptr;
		ptr++; fieldi++;
	}
	target->resource[fieldi] = 0;
	ptr++; fieldi = 0;
	while(*ptr != '\r'){
		target->protocol[fieldi] = *ptr;
		ptr++; fieldi++;
	}
	target->protocol[fieldi] = 0;
	ptr += 2; fieldi = 0; line++;
	
	while(*ptr != '\r'){	// CHANGE REALLOC TO BE BUFFERED FOR SPEED OPTIMIZATION
		field_t *hold = realloc(target->additionals, line * sizeof(field_t));
		if(hold) target->additionals = hold;
		else return NULL;	// ADD JUMP HERE FOR ERROR RECOVERY
		field_t newfield;
		memset(newfield.field, 0, 64);
		memset(newfield.content, 0, 256);
		while(*ptr != ':') {
			newfield.field[fieldi] = *ptr;
			ptr++; fieldi++;
		}
		newfield.field[fieldi] = 0;
		ptr += 2; fieldi = 0;
		while(*ptr != '\r'){
			newfield.content[fieldi] = *ptr;
			ptr++; fieldi++;
		}
		newfield.content[fieldi] = 0;
		ptr += 2; fieldi = 0;
		target->additionals[line - 1] = newfield;
		line++;
	}
	field_t *hold = realloc(target->additionals, line * sizeof(field_t));
	if(hold) target->additionals = hold;
	else return NULL;
	field_t newfield = {"end", "\0"};
	target->additionals[line - 1] = newfield;
	return target;
}

void delReqAttribs(req_t *attribs){
	free(attribs->additionals);
	free(attribs);
	return;
}

char *findFromAttrib(req_t *request, char *field){
	for(int i = 0; strcmp(request->additionals[i].field, "end"); i++){
		if(!strcmp(request->additionals[i].field, field)){
			return request->additionals[i].content;
		}
	}
	return NULL;
}

int findFromReq(char *content, char *value){
	return (strstr(content, value)) ? 1 : 0;
}

int findFromAttribField(req_t *request, char *field, char *value){
	char *found = NULL;
	for(int i = 0; strcmp(request->additionals[i].field, "end"); i++){
		if(!strcmp(request->additionals[i].field, field)){
			found = request->additionals[i].content;
			break;
		}
	}
	if(!found) return 0;
	return findFromReq(found, value);
}
