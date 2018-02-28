//
//  HTTPReq.c
//  servertest
//
//  Created by Kuura Parkkola on 28/02/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include "HTTPReq.h"

#include <string.h>
#include <stdlib.h>

req_t *HTTPCreate(char *request){
	req_t *target = calloc(1, sizeof(req_t));
	target->additionals = NULL;
	char *ptr = NULL;
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
	}
	return target;
}
