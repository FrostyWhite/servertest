//
//  pages.c
//  servertest
//
//  Created by Kuura Parkkola on 11/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include "pages.h"

rsc_t getPage(char *resource){
	rsc_t pages[] = {
		{error404, "/404/", "error404", "./pages/e404.html", ""},
		{about, "/about/", "about", "./pages/about.html", NULL},
		{mainpage, "/", "mainpage", "./pages/main.html", "./database/main.isd"},
		{indexpage, "/index/", "indexpage", "./pages/index.html", "./database/characters.isd"},
		{NULL, NULL, NULL, NULL, NULL}
	};
	
	for (int i = 0; pages[i].url; i++) {
		if((strlen(resource) == strlen(pages[i].url)) && !strcmp(resource, pages[i].url)){
			return pages[i];
		}
	}
	return pages[0];
}

char *error404(char *filepath, char *datapath, req_t *request){
	tag_t title = dataFromVA("{%%TITLE%%}", "title", "%1v", NULL, "404", NULL);
	
	tag_t content = dataFromVA("{%%CONTENT%%}", "h1", "%1v", NULL, "The resource you were looking for could not be found.", NULL);
	return createResponse(render_html(filepath, title, content, makeEnd()), "404");
}

char *about(char *filepath, char *datapath, req_t *request){
	tag_t title = dataFromVA("{%%TITLE%%}", "title", "%1v", NULL, "About", NULL);
	
	tag_t head = dataFromVA("{%%HEADER%%}", "h1", "%1v", NULL, "What is the Icicle server?", NULL);
	
	tag_t content = dataFromVA("{%%CONTENT%%}", "p", "%1v", NULL, "Icicle server is a simple web server base. Like a web server should, it accepts multiple connections and handles them efficiently. The Icicle web server project is flexibe and can be used to render any data. Even more so in the future as the project is continuously expanding", NULL);
	
	return createResponse(render_html(filepath, title, head, content, makeEnd()), "200");
}

char *mainpage(char *filepath, char *datapath, req_t *request){
	tag_t title = dataFromVA("{%%TITLE%%}", "title", "%1v", NULL, "Welcome", NULL);
	
	tag_t content = dataFromVA("{%%CONTENT%%}", "h1", "%1v", NULL, "Welcome to this demo page running Icicle Server!", NULL);
	return createResponse(render_html(filepath, title, content, makeEnd()), "200");
}



char *indexpage(char *filepath, char *datapath, req_t *request){
	tag_t title = dataFromVA("{%%TITLE%%}", "title", "%1v", NULL, "Actorpage", NULL);
	
	tag_t head = dataFromVA("{%%HEADER%%}", "h1", "%1v", NULL, "Cool actors", NULL);
	
	tag_t render = dataFromFile("{%%CONTENT%%}", "p", "%-15a %-+20a %-20a", datapath);
	
	return createResponse(render_html(filepath, title, head, render, makeEnd()), "200");
}
