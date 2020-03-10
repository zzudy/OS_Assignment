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
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#define MAX 128


void parse_command(const char* input,
                   int* argc, char*** argv)
{
   int i = 0;
   char *token;
   char *ptr = (char*)malloc(MAX * sizeof(char));
   char *re;
   *argv = (char**)malloc(MAX * sizeof(char*));
   strcpy(ptr, input);

   re = strchr(ptr, '\"');

   if (re == NULL) { //no "
      token = strtok(ptr, " \n\t");
      while (token != NULL) {
         (*argv)[i] = (char*)malloc(MAX * sizeof(char));
         strcpy((*argv)[i], token);
         token = strtok(NULL, " \n\t");
         i++;
      }
      (*argv)[i] = NULL;
      *argc = i;

    free(ptr);
   }
   else { // cat some.txt > \"what I am writing\"
      char **str = (char**)malloc(MAX * sizeof(char*));
      token = strtok(ptr, "\"\n\t");

      while (token != NULL) {
         str[i] = (char*)malloc(sizeof(char)*(strlen(token)+1));
         strcpy(str[i],token);
         token = strtok(NULL, "\"\n\t");
         i++;
      }
      int j = 0;
      token = strtok(str[0], " \n\t");

      while (token != NULL) {
         (*argv)[j] = (char*)malloc(MAX * sizeof(char));
         strcpy((*argv)[j], token);
         token = strtok(NULL, " \n\t");
         j++;
      }
      (*argv)[j] = (char*)malloc(MAX * sizeof(char));
      strcpy((*argv)[j], str[1]);
      *argc = j + 1;


    for (int k = 0; k <2; k++) {
     
      free((str)[(k)]);
      str[k]=NULL;
    }
    //*argc = j + 1;
    (*argv)[*argc] = NULL;
    free((str));

    free(ptr);
   }
   
    // if((*argv)[(*argc)-1][0] == '&'){ // delete &
    // printf("back11\n");
    // back = 1;

    // printf("%d\n", back);
    //   free((*argv)[(*argc)-1]);
    //   (*argv)[(*argc)-1]= NULL;
    //   (*argc)--;
    // }
   
  }