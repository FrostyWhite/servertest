//
//  HTTPReq.h
//  servertest
//
//  Created by Kuura Parkkola on 28/02/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#ifndef HTTPReq_h
#define HTTPReq_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
	char field[64];
	char content[256];
} field_t;

typedef struct{
	char method[16];
	char resource[128];
	char protocol[16];
	field_t *additionals;
}req_t;


char *handleRequest(char *request);

req_t *HTTPCreate(char *request);

void HTTPDelete(req_t *toDelete);

char *HTTPFind(req_t *request, char *field);

int HTTPStrSeek(char *content, char *value);

int HTTPReqSeek(req_t *request, char *field, char *value);

#endif /* HTTPReq_h */
