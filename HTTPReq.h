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

req_t *HTTPCreate(char *request);

int HTTPDelete(req_t toDelete);

int HTTPFind(req_t source, char *field);

#endif /* HTTPReq_h */
