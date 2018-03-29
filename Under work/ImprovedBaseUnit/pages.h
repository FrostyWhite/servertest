//
//  pages.h
//  servertest
//
//  Created by Kuura Parkkola on 11/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#ifndef pages_h
#define pages_h

#include <stdio.h>
#include "HTTPReq.h"
#include "render.h"

typedef struct{
	char *(*viewFunc)(char*, char*, req_t*);
	char *url;
	char *pagename;
	char *filepath;
	char *datapath;
} rsc_t;

rsc_t getPage(char *resource);

char *error404(char *filepath, char *datapath, req_t *request);
char *about(char *filepath, char *datapath, req_t *request);
char *mainpage(char *filepath, char *datapath, req_t *request);
char *indexpage(char *filepath, char *datapath, req_t *request);

#endif /* pages_h */
