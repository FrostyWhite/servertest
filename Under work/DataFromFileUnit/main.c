//
//  main.c
//  DataFromFileUnit
//
//  Created by Kuura Parkkola on 02/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
	char *fields; // fromatted string containing the data fields
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

typedef struct{
	short type;
	char flags;
	int presision;
	int width;
} hidden_format_t;

hidden_format_t hiddenCreateFormat(){
	hidden_format_t format = {0, 0, 0, 0};
	return format;
}

int hiddenCharsToDelim(const char *string, char delim){
	for(int chars = 0; chars < strlen(string); chars++){
		if(*(string + chars) == delim) return chars;
	}
	return 0;
}

void hiddenResetFormat(hidden_format_t *format){
	format->type = 0;
	format->flags = 0;
	format->presision = 0;
	format->width = 0;
}

void hiddenWriteChars(char *target, int number, char character){
	for(int i = 0; i < number; i++){
		*target = character;
		target++;
	}
	return;
}

// CURRENTLY ONLY ACCEPTS ABSOLUTE WIDTH
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

int hiddenNonFormattedWrite(const char *source, char *target, const char delim){
	int step = 0;
	while(*source != delim){
		*target = *source;
		target++; source++; step++;
	}
	return step;
}

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
							case '+': // sign
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

tag_t makeEnd(){
	tag_t end;
	end.parseFunc = NULL;
	end.cnt = 0;
	end.left = 0;
	end.data.fields = NULL;
	end.data.values = NULL;
	return end;
}

tag_t dataFromFile(char *base, char *tag, char *html_t, char *format){
	tag_t new; int linei = 0; int count = 0;
	FILE *file = fopen(base, "r");
	if(!file) return makeEnd();
	data_t data;
	char line[256];
	memset(line, 0, 256);
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
	new.parseFunc = NULL;
	return new;
}

int main(int argc, char *argv[]){
	tag_t packets = dataFromFile("./characters", "{%%CONTENT%%}", "p", "%-15a %-20l %-20h");
	printf("%s\n", packets.data.fields);
	for(int i = 0; i < packets.left; i++){
		printf("%s\n", packets.data.values[i]);
	}
	return 0;
}
