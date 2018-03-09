//
//  render.h
//  servertest
//
//  Created by Kuura Parkkola on 01/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#ifndef render_h
#define render_h

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Required tools and typedefinitions //

typedef struct{
	char *fields; // formatted string containing the data fields
	char **values; // formatted datastrings
} data_t;

typedef struct{
	data_t data; // Data to be rendered
	char tag[20]; // Tag to be replaced in html template
	char html_f[10]; // html tags to be added to the data for rendering
	unsigned int cnt; // how many datastrings contained in package
	unsigned int left; // how many datastrings left unprocessed
	char* (*parseFunc)(void *); // function used for rendering datastrings
} tag_t;

// typedefinition for easier utilization of function pointer
typedef char* (*parseFunc)(tag_t);

tag_t makeEnd();

// TODO: VARIABLE TABLE ROW LENGTH
tag_t dataFromFile(char *tag, char *html_t, char *format, char *filepath);

tag_t dataFromVA(char *tag, char *html_t, char *format, char **title, ...);// NOT IMPLEMENTED

tag_t dataFromList(char *tag, char *html_t, char *format, char **title, char **list);

// Renders the html document at 'path'
// filling it with the given data
char *render_html(char *path, ...);

// Takes taglines from given data and forms
// html tagged lines out of them
// *** TODO: FIX left update issue here and in render_html ***
char *parseTag(void *p_task);

#endif /* render_h */
