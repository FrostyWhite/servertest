//
//  render.c
//  servertest
//
//  Created by Kuura Parkkola on 01/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include "render.h"

////
//
//		HIDDEN TOOLS AND PROTOTYPES
//
////

typedef struct{
	short type;
	char flags;
	int presision;
	int width;
} hidden_format_t;

// Initializes a new formatting options structure
hidden_format_t hiddenInitFormat();

// Clears the given formatting options structure
void hiddenResetFormat(hidden_format_t *format);

// Formats a list of strings
int hiddenFormatLine(char **source, char *target, const char *format);

////
//
//	HANDLING FUNCTIONS
//
////

char *handleRequest(char *request){
	req_t *req_data = HTTPCreate(request);
	rsc_t pageinfo = getPage(req_data->resource);
	return pageinfo.viewFunc(pageinfo.filepath, pageinfo.datapath, req_data);
}

char *createResponse(char *document, char *code){
	char *response = calloc(512 + strlen(document), sizeof(char));
	switch (atoi(code)) {
		case 200:
			sprintf(response, "HTTP/1.1 200 OK\nServer: Icicle Server\nConnection: close\nContent-Type: text/html; charset=UTF-8\nContent-Length: %d\n\n%s\n\n", (int) strlen(document), document);
			free(document);
			return response;
			break;
		case 404:
			sprintf(response, "HTTP/1.1 404 Not Found\nContent-Length: %d\n\n%s\n\n", (int) strlen(document), document);
			free(document);
			return response;
			break;
		default:
			return NULL;
			break;
	}
	return NULL;
}

////
//
//	RENDERING FUNCTIONS
//
////

////
////	Takes taglines from given data and forms
////	html tagged lines out of them
////
//
//  Typecast the parameter datapacket,
//  check whether or not to print the title,
//  allocate memory for the line content
//  if there was a title line (it was printed):
//		free the title line so it won't be printed again
//  Return the line
char *parseTag(void *p_task){	// The basic parse function
	tag_t *task = (tag_t *) p_task;
	char *data = (task->data.fields != NULL) ? task->data.fields : task->data.values[task->cnt - task->left];
	
	char *parsed = calloc((2 * strlen(task->html_f) + strlen(data) + 7), sizeof(char));
	sprintf(parsed, "<%s>%s</%s>", task->html_f, data, task->html_f);

	if(task->data.fields != NULL){
		free(task->data.fields);
		task->data.fields = NULL;
	}
	return parsed;
}

////
////	Renders the html document at 'path'
////	filling it with the given data
////
//
//  Set buffersize for memory allocations
//  Set up variable argument list
//  Set up resources
//  Pick first variable argument
//  Go through html template line by line:
//		if the tag from va-arg list found:
//			Go through all text blocks in the va-arg datablock:
//				Calculate line length
//				Allocate data if necessary
//				Write data to the rendered html document
//				Clean up already processed data
//			if datablock is fully processed:
//				Clean up processed datablock
//				Load a new datablock
//		else (line read from file doesn't contain tags):
//			Allocate memory if necessary
//			Write the line to the html document
//		Reset the line read from file so that the next can be read
//	Close the file and return the rendered document
char *render_html(char *path, ...){
	const unsigned int buffersize = 2048;
	
	va_list templatetags;
	va_start(templatetags, path);
	
	FILE *fp = fopen(path, "r");
	char *render = calloc(buffersize, sizeof(char));
	unsigned int left = buffersize;
	
	tag_t processed = va_arg(templatetags, tag_t);
	
	char line[256]; // Lines can't realistically be this long
	while (fgets(line, 255, fp) != NULL) {
		char *loc = 0;
		
		if ((loc = strstr(line, processed.tag))){

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
				left -= 2 + strlen(line + strlen(tagdata));

				free(processed.data.values[processed.cnt - processed.left]);
				processed.left--;
			}

			if (processed.cnt > 0) {
				free(processed.data.values);
				processed = va_arg(templatetags, tag_t);
			}
		}

		else {

			while(left < strlen(line)){
				render = realloc(render, sizeof(render) + buffersize * sizeof(char));
				left += buffersize;
			}

			strcat(render, line);
			left -= 1 + strlen(line);
		}

		memset(line, 0, 256);
	}

	fclose(fp);
	return render;
}

////
////	Creates an end tag for later use
////
//
//	Create new tag
//	Set all values clear
tag_t makeEnd(){
	tag_t end;
	end.parseFunc = NULL;
	end.cnt = 0;
	end.left = 0;
	end.data.fields = NULL;
	end.data.values = NULL;
	return end;
}

////
//
//	FORMATTING FUNCTIONS
//
////

////
////	Creates a datapacket from a file
////
//
//	Create resources
//	Open file or return an end tag
//	Count number of fields on a line
//	Read a line from file
//	If the line contains field tag:
//		Skip over the tag
//		Loop through the line until no more delimiters found:
//			Set the delimiter 0 (split the line)
//			Allocate space for the new field
//			Copy the current line partial to the field list
//			Set the start of the next partial behind the end of the last one
//		Allocate space for a trailing NULL element
//		Allocate space for tagstruck fields field
//		Reset the line and read a new one
//	Set up datagroup buffer
//	Loop through each line of the file:
//		Set the start of the first line partial to the beginning of the line
//		Loop through the line until no more delimiters found:
//			If datagroup buffer full:
//				Allocate more space
//				Clear allocated space
//			If current datagroup slot not empty: clear slot
//			Else: allocate space into the slot
//			Set the delimiter 0 (split the line)
//			Copy the current string partial to the current datagroup slot
//			Set the start of the next partial behind the end of the last one
//			(new line loaded)
//		If datagroup buffer full:
//			Allocate one more slot
//			Null the slot
//		Else: Null the current slot
//		Allocate space to the datastruct
//		Format the datagroup into the datastruct
//	Free any used slots in the datagroup
//	Free the datagroup
//	Close the file
//	Fill the tagstruct
//	Return the tagstruct
tag_t dataFromFile(char *tag, char *html_t, char *format, char *filepath){
	
	tag_t new;
	int count = 0; int lines = 0;
	char *step; char *drag;
	char line[256];
	memset(line, 0, 256);
	
	FILE *file = fopen(filepath, "r");
	if(!file) return makeEnd();
	
	new.data.fields = NULL;
	fgets(line, 255, file);
	if(strstr(line, "%FIELDS%") == line){
		drag = line + 8;
		
		char **title = NULL;
		while((step = strchr(drag, ';'))){
			*step = 0;
			
			title = realloc(title, (count + 1) * sizeof(char *));
			title[count] = calloc(1 + strlen(drag), sizeof(char));
			
			strcpy(title[count], drag);
			count++;
			drag = step + 1;
		}
		title = realloc(title, (count + 1) * sizeof(char *));
		title[count] = NULL;
		
		new.data.fields = calloc(512, sizeof(char));
		hiddenFormatLine(title, new.data.fields, format);
		memset(line, 0, 256);
		fgets(line, 255, file);
		count = 0;
	}
	
	new.data.values = NULL;
	const int dtgrpbuf = 10;
	int dtgrp_inuse = 0;
	int dtgrp_left = dtgrpbuf;
	char **datagroup = calloc(dtgrpbuf, sizeof(char *));
	for (int i = 0; i < dtgrpbuf; i++){
		datagroup[dtgrp_inuse + i] = NULL;
	}
	
	do{
		drag = line;
		while((step = strchr(drag, ';'))){
			if(dtgrp_left == 0){
				datagroup = realloc(datagroup, (dtgrp_inuse + dtgrpbuf) * sizeof(char *));
				for (int i = 0; i < dtgrpbuf; i++){
					datagroup[dtgrp_inuse + i] = NULL;
				}
				dtgrp_left = dtgrpbuf;
			}
			if(datagroup[count]) memset(datagroup[count], 0, 128 * sizeof(char));
			else{	datagroup[count] = calloc(128, sizeof(char));
				dtgrp_inuse++; dtgrp_left--;	}
			
			*step = 0;
			strcpy(datagroup[count++], drag);
			drag = step + 1;
		}
		if(dtgrp_left == 0){
			datagroup = realloc(datagroup, (dtgrp_inuse + 1) * sizeof(char *));
			datagroup[dtgrp_inuse++] = NULL;
		}
		else datagroup[count] = NULL;
		
		new.data.values = realloc(new.data.values, (lines + 1) * sizeof(char*));
		new.data.values[lines] = calloc(512, sizeof(char));
		hiddenFormatLine(datagroup, new.data.values[lines++], format);
		count = 0;
	} while(fgets(line, 255, file));
	
	for (int i = 0; i < dtgrp_inuse; i++) {
		if(datagroup[i]) free(datagroup[i]);
	}
	free(datagroup);
	
	fclose(file);
	
	new.cnt = lines;
	new.left = lines;
	new.parseFunc = parseTag;
	memset(new.tag, 0, 20);
	memset(new.html_f, 0, 10);
	if(tag) strcpy(new.tag, tag);
	if(html_t) strcpy(new.html_f, html_t);
	
	return new;
}

////
////	Creates a datapacket from a variable argument list
////
//
//	Create resources
//	Count number of fields on a line
//	If a title list is given:
//		Format the title list into datastruct
//	Load first vararg
//	Set up datagroup buffer
//	Loop as long as varargs last:
//		Loop through enough values to fill a line:
//			If datagroup buffer full:
//				Allocate more space
//				Clear allocated space
//			If current datagroup slot not empty: clear slot
//			Else: allocate space into the slot
//			Copy vararg to the current datagroup slot
//			Load the next vararg
//		If datagroup buffer full:
//			Allocate one more slot
//			Null the slot
//		Else: Null the current slot
//		Allocate space to the datastruct
//		Format the datagroup into the datastruct
//	Free any used slots in the datagroup
//	Free the datagroup
//	Fill the tagstruct
//	Return the tagstruct
tag_t dataFromVA(char *tag, char *html_t, char *format, char **title, ...){
	
	tag_t new;
	va_list va_strings;
	va_start(va_strings, title);
	int line_elements = 0;
	int lines = 0; int le;
	
	char *scan = format;
	while(*scan) if(*(scan++) == '%' && *(scan + 1) != '%') line_elements++;
	
	new.data.fields = NULL;
	if(title) {
		hiddenFormatLine(title, new.data.fields, format);
	}
	
	char *va_str = va_arg(va_strings, char*);
	
	const int dtgrpbuf = 10;
	int dtgrp_inuse = 0;
	int dtgrp_left = dtgrpbuf;
	char **datagroup = calloc(dtgrpbuf, sizeof(char *));
	for (int i = 0; i < dtgrpbuf; i++){
		datagroup[dtgrp_inuse + i] = NULL;
	}
	new.data.values = NULL;
	
	while(va_str){
		for(le = 0; (le < line_elements) && va_str; le++){
			if(dtgrp_left == 0){
				datagroup = realloc(datagroup, (dtgrp_inuse + dtgrpbuf) * sizeof(char *));
				for (int i = 0; i < dtgrpbuf; i++){
					datagroup[dtgrp_inuse + i] = NULL;
				}
				dtgrp_left = dtgrpbuf;
			}
			if(datagroup[le]) memset(datagroup[le], 0, 128 * sizeof(char));
			else{	datagroup[le] = calloc(128, sizeof(char));
				dtgrp_inuse++; dtgrp_left--;	}
			
			strcpy(datagroup[le], va_str);
			va_str = va_arg(va_strings, char*);
		}
		
		if(dtgrp_left == 0){
			datagroup = realloc(datagroup, (dtgrp_inuse + 1) * sizeof(char *));
			datagroup[dtgrp_inuse++] = NULL;
		}
		else datagroup[le] = NULL;
		
		new.data.values = realloc(new.data.values, (lines + 1) * sizeof(char*));
		new.data.values[lines] = calloc(512, sizeof(char));
		hiddenFormatLine(datagroup, new.data.values[lines++], format);
	}
	
	for (int i = 0; i < dtgrp_inuse; i++) {
		if(datagroup[i]) free(datagroup[i]);
	}
	free(datagroup);
	
	new.cnt = lines;
	new.left = lines;
	new.parseFunc = parseTag;
	if(tag) strcpy(new.tag, tag);
	if(html_t) strcpy(new.html_f, html_t);
	
	return new;
}

////
////	Creates a datapacket from a list
////
//
//	Create resources
//	Count number of fields on a line
//	If a title list is given:
//		Format the title list into datastruct
//	Set up datagroup buffer
//	Loop as long as there are list elements:
//		Loop through enough values to fill a line:
//			If datagroup buffer full:
//				Allocate more space
//				Clear allocated space
//			If current datagroup slot not empty: clear slot
//			Else: allocate space into the slot
//			Copy the current list element to the current datagroup slot
//			Take the next element on the list
//		If datagroup buffer full:
//			Allocate one more slot
//			Null the slot
//		Else: Null the current slot
//		Allocate space to the datastruct
//		Format the datagroup into the datastruct
//	Free any used slots in the datagroup
//	Free the datagroup
//	Fill the tagstruct
//	Return the tagstruct
tag_t dataFromList(char *tag, char *html_t, char *format, char **title, char **list){
	
	tag_t new;
	int line_elements = 0;
	int lines = 0;
	
	char *scan = format;
	while(*scan) if(*(scan++) == '%' && *(scan + 1) != '%') line_elements++;
	
	new.data.fields = NULL;
	if(title) {
		hiddenFormatLine(title, new.data.fields, format);
	}
	
	int le;
	
	const int dtgrpbuf = 10;
	int dtgrp_inuse = 0;
	int dtgrp_left = dtgrpbuf;
	char **datagroup = calloc(dtgrpbuf, sizeof(char *));
	for (int i = 0; i < dtgrpbuf; i++){
		datagroup[dtgrp_inuse + i] = NULL;
	}
	new.data.values = NULL;
	
	while(*list){
		for(le = 0; (le < line_elements) && *list; le++){
			if(dtgrp_left == 0){
				datagroup = realloc(datagroup, (dtgrp_inuse + dtgrpbuf) * sizeof(char *));
				for (int i = 0; i < dtgrpbuf; i++){
					datagroup[dtgrp_inuse + i] = NULL;
				}
				dtgrp_left = dtgrpbuf;
			}
			if(datagroup[le]) memset(datagroup[le], 0, 128 * sizeof(char));
			else{	datagroup[le] = calloc(128, sizeof(char));
				dtgrp_inuse++; dtgrp_left--;	}
			
			strcpy(datagroup[le], *(list++));
		}
		
		if(dtgrp_left == 0){
			datagroup = realloc(datagroup, (dtgrp_inuse + 1) * sizeof(char *));
			datagroup[dtgrp_inuse++] = NULL;
		}
		else datagroup[le] = NULL;
		
		
		new.data.values = realloc(new.data.values, (lines + 1) * sizeof(char*));
		new.data.values[lines] = calloc(512, sizeof(char));
		hiddenFormatLine(datagroup, new.data.values[lines++], format);
	}
	
	for (int i = 0; i < dtgrp_inuse; i++) {
		if(datagroup[i]) free(datagroup[i]);
	}
	free(datagroup);
	
	new.cnt = lines;
	new.left = lines;
	new.parseFunc = parseTag;
	if(tag) strcpy(new.tag, tag);
	if(html_t) strcpy(new.html_f, html_t);
	
	return new;
}

////
//
//	HIDDEN UTILITIES
//
////

////
////	Creates an empty formatting options struct
////
hidden_format_t hiddenInitFormat(){
	hidden_format_t format = {0, 0, 0, 0};
	return format;
}

////
////	Clears the given formatting options struct
////
void hiddenResetFormat(hidden_format_t *format){
	format->type = 0;
	format->flags = 0;
	format->presision = 0;
	format->width = 0;
}

////
////	Write to target buffer from format and source
////	buffers based on instructions in format buffer
////
//
//	Set some resources
//  Loop through format string:
//		Pick actions for the current character
//		Case %: (character starts format tag)
//			Loop until format type is specified:
//				If formatting width not specified:
//					Set correct option for first formatting flag
//					If a number is found it has to define field width
//					If no width is specified and random character met function fails
//				Else: Next character has to define formatting type or function fails
//					Pick the corresponding character
//					Format string correctly based on saved options
//				Move to the next source list element
//		Case //:( character is a special character)
//			write the correct character to target
//		By default write the character to target
//	Add trailing 0
//  Return formatted string
int hiddenFormatLine(char **source, char *target, const char *format){
	int srcind = 0;
	hidden_format_t settings = hiddenInitFormat();
	const char *tmp = NULL;
	
	while (*format) {
		hiddenResetFormat(&settings);
		switch (*format) {
			case '%':
				format++;
				while (!settings.type) {
					if(!settings.width){
						switch (*format) {
							case '-': // left allignment
								if(settings.flags & 8) settings.flags &= ~(8);
								settings.flags |= 1;
								format++;
								break;
							case '+': // sign //NOTE FIX FILL CHAR ERROR
								if(settings.flags & 4) settings.flags &= ~(4);
								settings.flags |= 2;
								format++;
								break;
							case ' ': // sign space
								format++;
								if(settings.flags & 4) break;
								settings.flags |= 4;
								break;
							case '0': // prepend zeros
								format++;
								if(settings.flags & 1) break;
								settings.flags |= 8;
							default:
								if(*format >= 49 && *format <= 57){
									tmp = format;
									while(*(format++) >= 48 && *format <= 57);
									settings.width = atoi(tmp);
								}
								else if (*(format++) == 'v'){
									settings.width = -1;
									settings.type = 4;
								}
								else return 0;
								break;
						}
					}
					else{
						int fill = 0;
						switch (*(format++)) {
							case 'l':
								settings.type = 1; // Low bound
								fill = settings.width - (int) strlen(source[srcind]);
								if(fill < 0){
									strcpy(target, source[srcind]);
									target += strlen(source[srcind]);
								}
								else{
									if(settings.flags & 1){
										strcpy(target, source[srcind]);
										target += strlen(source[srcind]);
										memset(target, (settings.flags & 8) ? '0' : ' ', fill);
										*(target += fill) = 0;
									}
									else{
										memset(target, (settings.flags & 8) ? '0' : ' ', fill);
										*(target += fill) = 0;
										strcpy(target, source[srcind]);
										target += strlen(source[srcind]);
									}
								}
								break;
							case 'h':
								settings.type = 2; // High bound
								strncpy(target, source[srcind], settings.width);
								target += (strlen(source[srcind]) < settings.width) ? strlen(source[srcind]) : settings.width;
								break;
							case 'a':
								settings.type = 3; // Absolute
								fill = settings.width - (int) strlen(source[srcind]);
								if(fill < 0){
									strncpy(target, source[srcind], settings.width);
									target += (strlen(source[srcind]) < settings.width) ? strlen(source[srcind]) : settings.width;
								}
								else{
									if(settings.flags & 1){
										strcpy(target, source[srcind]);
										target += strlen(source[srcind]);
										memset(target, (settings.flags & 8) ? '0' : ' ', fill);
										*(target += fill) = 0;
									}
									else{
										memset(target, (settings.flags & 8) ? '0' : ' ', fill);
										*(target += fill) = 0;
										strcpy(target, source[srcind]);
										target += strlen(source[srcind]);
									}
								}
								break;
							case 'v':
								settings.type = 4; // Variable
								strcpy(target, source[srcind]);
								target += strlen(source[srcind]);
								break;
							default:
								return 0;
								break;
						}
					}
				}
				srcind++;
				break;
			default:
				*(target++) = *(format++);
				break;
		}
	}
	*target = 0;
	return 1;
}

