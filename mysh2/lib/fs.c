/**********************************************************************
 * Copyright (C) Jaewon Choi <jaewon.james.choi@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 *********************************************************************/
#include "fs.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include <stdio.h>

int does_exefile_exists(const char* path)
{
  // TODO: Fill it!
if(strchr(path,'/') != 0){
  int r= access(path,X_OK);
  
  if(r==0){
    return 1;
  }
  else{
    return 0;
    }
}


else{
  char* repath = getenv("PATH");
  if(resolve_path(path,repath)==1){
    return 1;
  }
  return 0;
 }
}


int resolve_path(const char* Path, char* Pathname){
	char *token;
	char *ptr=(char*)malloc(1000 * sizeof(char));
	char *re;
	strcpy(ptr, Pathname);

	token = strtok(ptr, ":");

	char buffer1[512];

	while (token != NULL) {
    sprintf(buffer1,"%s/%s",token,Path);
    int r = access(buffer1,X_OK);
    if(r==0){
      free(ptr);
      return 1;
    }
		token = strtok(NULL, ":");
	}
  free(ptr);

	return 0;

}
