//
//  main.c
//  ImprovedDatastructUnit
//
//  Created by Kuura Parkkola on 07/03/2018.
//  Copyright © 2018 Kuura Parkkola. All rights reserved.
//

#include <stdio.h>
#include "render.h"

int main(int argc, const char * argv[]) {
	tag_t data1 = dataFromFile(NULL, NULL, "%-20a %-20a %-20a", "./characters");
	for(int i = 0; i < data1.left; i++){
		printf("%s\n", data1.data.values[i]);
	}
	tag_t data2 = dataFromVA(NULL, NULL, "%-20a", NULL, "VALUE", "value", "Another!", NULL);
	for(int i = 0; i < data2.left; i++){
		printf("%s\n", data2.data.values[i]);
	}
	char *list[] = {"VALUE", "value", "Another!", NULL};
	tag_t data3 = dataFromList(NULL, NULL, "%-20a", NULL, list);
	for(int i = 0; i < data3.left; i++){
		printf("%s\n", data3.data.values[i]);
	}
    return 0;
}
