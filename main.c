//
//  main.c
//  iOS Restrictions Cracker
//
//  Created by Alex Taffe on 9/3/17.
//  Copyright © 2017 Alex Taffe. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "crack.h"

int main(int argc, char * argv[]){
	if(argc != 3){
		printf("Incorrect number of arguments not provided. Usage: restrictions-crack <hash> <salt>\n");
		exit(1);
	}
	char *hash = argv[1];
	char *salt = argv[2];

	char **hashes = (char **)malloc(sizeof(char *));
	char **salts = (char **)malloc(sizeof(char *));

	hashes[0] = hash;
	salts[0] = salt;

	crackCode(hashes, salts, NULL);

}
