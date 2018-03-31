//
//  request.h
//  Version2Unit
//
//  Created by Kuura Parkkola on 31/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#ifndef request_h
#define request_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "datatypes.h"
#include "page.h"

char *processRequest(char *request);

char *generateResponse(char *content, char *status);

req_t *getReqAttribs(char *request);

void delReqAttribs(req_t *attribs);

char *findFromAttrib(req_t *request, char *field);

int findFromReq(char *content, char *value);

int findFromAttribField(req_t *request, char *field, char *value);

#endif /* request_h */
