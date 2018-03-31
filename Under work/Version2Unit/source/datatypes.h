//
//  datatypes.h
//  Version2Unit
//
//  Created by Kuura Parkkola on 01/04/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#ifndef datatypes_h
#define datatypes_h

////////////////////////
//	REQUEST HANDLING  //
////////////////////////

typedef struct{
	char field[64];
	char content[512];
} field_t;

typedef struct{
	char method[16];
	char resource[128];
	char protocol[16];
	field_t *additionals;
}req_t;

////////////////////////
//	PAGE ATTRIBUTES   //
////////////////////////

typedef struct{
	char *(*viewFunc)(char*, char*, req_t*);
	char *url;
	char *pagename;
	char *filepath;
	char *datapath;
} rsc_t;

#endif /* datatypes_h */
