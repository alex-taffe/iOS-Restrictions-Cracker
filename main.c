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

	crackCode(hash, salt, NULL);

}
