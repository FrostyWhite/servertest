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
int *hiddenWriteFromSource(const char *source, char *target, const hidden_format_t *settings, const char delim);
int hiddenNonFormattedWrite(const char *source, char *target, const char delim);
int hiddenFormatLine(const char *source, char *target, const char *format, char delim);

////
//
//	PUBLIC FUNCTIONS
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
tag_t dataFromFile(char *base, char *tag, char *html_t, char *format){
	
	tag_t new; int linei = 0; int count = 0;
	data_t data; char line[256];
	memset(line, 0, 256);
	
	FILE *file = fopen(base, "r");
	if(!file) return makeEnd();
	
	fgets(line, 255, file);
	if(strstr(line, "%FIELDS%") == line){
		linei += 8;
		data.fields = calloc(1024, sizeof(char));
		hiddenFormatLine(&line[linei], data.fields, format, ';');
		memset(line, 0, 256);
		fgets(line, 255, file);
	}
	data.values = NULL;
	
	do {
		data.values = realloc(data.values, (count + 1) * sizeof(char*));
		data.values[count] = calloc(512, sizeof(char));
		hiddenFormatLine(line, data.values[count], format, ';');
		count++;
		memset(line, 0, 256);
	} while (fgets(line, 255, file));
	new.data = data;
	new.cnt = count;
	new.left = count;
	strcpy(new.html_f, html_t);
	strcpy(new.tag, tag);
	new.parseFunc = parseTag;
	return new;
}

/*
tag_t dataFromInput(char *tag, char *html_t, char *format, char *title, ...){
	
	tag_t new;
	va_list va_strings;
	va_start(va_strings, title);
	int line_elements = 0;
	int linei = 0; int count = 0;
	char source[4096];
	memset(source, 0, 4096);
	
	while(*format){
		if(*format == '%' && *(format + 1) != '%') line_elements++;
		format++;
	}
	
	if(title) {
		new.data.fields = calloc(1 + strlen(title), sizeof(char));
		strcpy(new.data.fields, title);
	}
	
	char *processed = va_arg(va_strings, char*);
	while (processed) {
		for(int le = 0; (le < line_elements) && processed; le++){
			strcat(source, processed);
			strcat(source, ";");
			processed = va_arg(va_strings, char*);
		}
		hiddenFormatLine(source, , , )
	}
	return new;
}
*/

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
////	Calculates the number of characters
////	to the delimiter
////
int hiddenCharsToDelim(const char *string, char delim){
	for(int chars = 0; chars < strlen(string); chars++){
		if(*(string + chars) == delim) return chars;
	}
	return 0;
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
////	Writes a specified number of the given
////	character into the given buffer
////
void hiddenWriteChars(char *target, int number, char character){
	for(int i = 0; i < number; i++){
		*target = character;
		target++;
	}
	return;
}

////
////	Writes text from source buffer to the
////	target buffer in a formatted form
////
//
//	Create some resources
//	Loop through the source until max length or delimiter:
//		If number formatting enabled and numerics found:
//			If number not preceeded by - add ' ' or + (based on options)
//			Else do nothing
//			Loop until whole number printed
//		In general case write the character to target
//	If max length reached before delimiter:
//		skip characters until delimiter
//	Return how many characters advanced in buffers
int *hiddenWriteFromSource(const char *source, char *target, const hidden_format_t *settings, const char delim){
	
	int *ret = calloc(2, sizeof(int));
	int sign = settings->flags & 2;
	int space = settings->flags & 4;
	unsigned int targetstep = 0;
	unsigned int sourcestep = 0;
	
	while(targetstep < settings->width && *source != delim){
		
		if((sign || space) && (*source >= 49 && *source <= 57)){
			
			target--;
			if(*target != '-'){
				target++;
				*target = (sign) ? '+' : ' ';
				targetstep++; target++;
				
			} else target++;
			
			while(*source >= 48 && *source <= 57 && targetstep < settings->width){
				*target = *source;
				source++; target++; targetstep++;
			}
			
		} else {
			*target = *source;
			source++; target++; targetstep++;
		}
	}
	
	sourcestep = targetstep;
	if(targetstep >= settings->width) {
		while(*source != delim){
			source++; sourcestep++;
		}
		source++; sourcestep++;
	}
	else{
		source++; sourcestep++;
	}
	
	ret[0] = targetstep;
	ret[1] = sourcestep;
	return ret;
}

////
////	Standard non-formatted write when no
////	formatting options presented
////
//
//	Write characters from source until delimiter
//	Return how much buffers advanced
int hiddenNonFormattedWrite(const char *source, char *target, const char delim){
	int step = 0;
	while(*source != delim){
		*target = *source;
		target++; source++; step++;
	}
	return step;
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
int hiddenFormatLine(const char *source, char *target, const char *format, char delim){
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
						switch (*format) {
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
				if(settings.width < 0 || settings.type == 4){
					int skip = hiddenNonFormattedWrite(source, target, delim);
					source += skip; target += skip;
					source++;
				} else {
					int fit = settings.width - hiddenCharsToDelim(source, delim);
					if(fit > 0){
						if(settings.type == 2) {
							int skip = hiddenNonFormattedWrite(source, target, delim);
							source += skip; target += skip;
							source++;
						}
						else if(settings.flags & 1){
							int *adv = hiddenWriteFromSource(source, target, &settings, delim);
							target += adv[0];
							source += adv[1];
							free(adv);
							hiddenWriteChars(target, fit, (settings.flags & 8) ? '0' : ' ');
							target += fit;
						} else {
							hiddenWriteChars(target, fit, (settings.flags & 8) ? '0' : ' ');
							target += fit;
							int *adv = hiddenWriteFromSource(source, target, &settings, delim);
							target += adv[0];
							source += adv[1];
							free(adv);
						}
					} else {
						if(settings.type == 1){
							int skip = hiddenNonFormattedWrite(source, target, delim);
							source += skip; target += skip;
							source++;
						} else {
							int *adv = hiddenWriteFromSource(source, target, &settings, delim);
							target += adv[0];
							source += adv[1];
							free(adv);
						}
					}
				}
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
