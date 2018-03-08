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

hidden_format_t hiddenCreateFormat();
int hiddenCharsToDelim(const char *string, char delim);
void hiddenResetFormat(hidden_format_t *format);
void hiddenWriteChars(char *target, int number, char character);
int *hiddenWriteFromSource(const char *source, char *target, const hidden_format_t *settings);
int hiddenNonFormattedWrite(const char *source, char *target);
int hiddenFormatLine(char **source, char *target, const char *format);

////
//
//	PUBLIC FUNCTIONS
//
////


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
////	Creates a datapacket from a given file
////
//
//	Creating resources
//	Open file or return only an end tag
//	If a title is marked in data file:
//		Format line
//		Reset line for a new read
//	Loop through the file:
//		Format the read line
//		Reset the line for a new read
//	Store the data inside the datapacket
//	Return the datapacket
tag_t dataFromFile(char *tag, char *html_t, char *format, char *filepath){
	
	tag_t new;
	FILE *file = fopen(filepath, "r");
	if(!file) return makeEnd();
	
	int count = 0; int lines = 0;
	char line[256];
	char *step; char *drag;
	memset(line, 0, 256);
	
	fgets(line, 255, file);
	if(strstr(line, "%FIELDS%") == line){
		char **title = NULL;
		char *titlestr;
		drag = line + 8;
		while((step = strchr(drag, ';'))){
			title = realloc(title, (count + 1) * sizeof(char *));
			*step = 0;
			titlestr = calloc(1 + strlen(drag), sizeof(char)); // direct index this
			strcpy(titlestr, drag);
			title[count] = titlestr;
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
			strcpy(datagroup[count], drag);
			drag = step + 1;
			count++;
		}
		if(dtgrp_left == 0){
			datagroup = realloc(datagroup, (dtgrp_inuse + 1) * sizeof(char *));
			datagroup[dtgrp_inuse] = NULL;
			dtgrp_inuse++;
		}
		else datagroup[count] = NULL;
		
		new.data.values = realloc(new.data.values, (lines + 1) * sizeof(char*));
		new.data.values[lines] = calloc(512, sizeof(char));
		hiddenFormatLine(datagroup, new.data.values[lines], format);
		lines++; count = 0;
	} while(fgets(line, 255, file));
	
	for (int i = 0; i < dtgrp_inuse; i++) {
		if(datagroup[i]) free(datagroup[i]);
	}
	free(datagroup);

	new.cnt = lines;
	new.left = lines;
	new.parseFunc = NULL;
	memset(new.tag, 0, 20);
	memset(new.html_f, 0, 10);
	if(tag) strcpy(new.tag, tag);
	if(html_t) strcpy(new.html_f, html_t);
	
	return new;
}

tag_t dataFromVA(char *tag, char *html_t, char *format, char **title, ...){
	tag_t new;
	va_list va_strings;
	va_start(va_strings, title);
	
	int line_elements = 0;
	int lines = 0;
	
	char *scan = format;
	while(*scan){
		if(*scan == '%' && *(scan + 1) != '%') line_elements++;
		scan++;
	}
	
	if(title) {
		hiddenFormatLine(title, new.data.fields, format);
	}
	
	int le;
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
			datagroup[dtgrp_inuse] = NULL;
			dtgrp_inuse++;
		}
		else datagroup[le] = NULL;
		
		for (int i = 0; i < le; i++) {
			printf("%d, %d : %s\n", lines, le, datagroup[0]);
		}
		new.data.values = realloc(new.data.values, (lines + 1) * sizeof(char*));
		new.data.values[lines] = calloc(512, sizeof(char));
		hiddenFormatLine(datagroup, new.data.values[lines], format);
		printf("AFT: %s\n", new.data.values[lines]);
		lines++;
	}
	
	for (int i = 0; i < dtgrp_inuse; i++) {
		if(datagroup[i]) free(datagroup[i]);
	}
	free(datagroup);
	
	new.cnt = lines;
	new.left = lines;
	new.parseFunc = NULL;
	if(tag) strcpy(new.tag, tag);
	if(html_t) strcpy(new.html_f, html_t);
	
	return new;
}

////
//
//	HIDDEN UTILITY FUNCTIONS
//
////

////
////	Creates an empty formatting options struct
////
hidden_format_t hiddenCreateFormat(){
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
//  go through format string:
//		if character starts format tag:
//			get formatting options
//			write formatted data from source to target
//		if character is a special character:
//			write the correct character to target
//		otherwise write character to target
//  return formatted string
int hiddenFormatLine(char **source, char *target, const char *format){
	int srcind = 0;
	hidden_format_t settings = hiddenCreateFormat();
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
									while(*format >= 48 && *format <= 57){
										format++;
									}
									settings.width = atoi(tmp);
								}
								else if (*format == 'v'){
									settings.width = -1;
									settings.type = 4;
									format++;
								}
								else return 0;
								break;
						}
					}
					else{
						switch (*format) { // THIS HERE AND
							case 'l':
								settings.type = 1; // Low bound
								break;
							case 'h':
								settings.type = 2; // High bound
								break;
							case 'a':
								settings.type = 3; // Absolute
								break;
							case 'v':
								settings.type = 4; // Variable
								break;
							default:
								return 0;
								break;
						}
						format++;
					}
				}
				if(settings.width < 0) settings.width = 0;
				if(settings.presision < 0) settings.presision = 0;
				int fill = settings.width - (int) strlen(source[srcind]);
				switch (settings.type) { // THIS HERE COULD BE DONE BETTER (COMBINED)!!
					case 1:
						if(fill < 0){
							strcpy(target, source[srcind]);
							target += strlen(source[srcind]);
						}
						else{
							if(settings.flags & 1){
								strcpy(target, source[srcind]);
								target += strlen(source[srcind]);
								memset(target, (settings.flags & 8) ? '0' : ' ', fill);
								target += fill;
								*target = 0;
							}
							else{
								memset(target, (settings.flags & 8) ? '0' : ' ', fill);
								target += fill;
								*target = 0;
								strcpy(target, source[srcind]);
								target += strlen(source[srcind]);
							}
						}
						break;
					case 2:
						strncpy(target, source[srcind], settings.width);
						target += (strlen(source[srcind]) < settings.width) ? strlen(source[srcind]) : settings.width;
						break;
					case 3:
						if(fill < 0){
							strncpy(target, source[srcind], settings.width);
							target += (strlen(source[srcind]) < settings.width) ? strlen(source[srcind]) : settings.width;
						}
						else{
							if(settings.flags & 1){
								strcpy(target, source[srcind]);
								target += strlen(source[srcind]);
								memset(target, (settings.flags & 8) ? '0' : ' ', fill);
								target += fill;
								*target = 0;
							}
							else{
								memset(target, (settings.flags & 8) ? '0' : ' ', fill);
								target += fill;
								*target = 0;
								strcpy(target, source[srcind]);
								target += strlen(source[srcind]);
							}
						}
						break;
					default:
						strcpy(target, source[srcind]);
						target += strlen(source[srcind]);
						break;
				}
				srcind++;
				break;
			case '\\':
				format++;
				switch (*format) {
					case 'n':
						*target = '\n';
						target++; format++;
						break;
					case 't':
						*target = '\t';
						target++; format++;
						break;
					default:
						*target = *format;
						target++; format++;
						break;
				}
				break;
			default:
				*target = *format;
				target++; format++;
				break;
		}
	}
	*target = 0;
	return 1;
}
