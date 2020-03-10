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
#include "commands.h"
#include "parser.h"
#include "utils.h"
#include "fs.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void signal_handler(int signo);
void signal_generator(int signo);

int main()

{

  char command_buffer[4096] = { 0, };

    back_check = (int*)malloc(sizeof(int));
    (*back_check) = 1;
    m = (*back_check);

    while (1) {
      if(back_check == NULL){
       back_check = (int*)malloc(sizeof(int));
        (*back_check) = m;
      }
        
     signal(SIGINT, signal_handler);
     signal(SIGTSTP, signal_handler);
     signal(SIGCHLD, signal_handler);

    if(fgets(command_buffer, 4096, stdin) == NULL){
      printf("\n");
      free(back_check);
      back_check = NULL;
      continue;
    }

    int argc = -1;
    char** argv = NULL;

    parse_command(command_buffer, &argc, &argv);

    if(argv[argc-1][0] == '&'){ // delete &
      back = 1;
      free(argv[argc-1]);
      argv[argc-1]= (char*)NULL;
      argc--;
    }else{
      back=0;
    }

    assert(argv != NULL);

    //printf("pid = %d\n", getpid()); 

    if (strcmp(argv[0], "kill") == 0) {
      
      if(argc!=2){
        m = (*back_check);
        free(back_check);
        back_check = (int*)NULL;

      FREE_2D_ARRAY(argc, argv);
        continue;
      }
      signal_generator(atoi(argv[1]));
      
      m = (*back_check);
      free(back_check);
      back_check = (int*)NULL;

      FREE_2D_ARRAY(argc, argv);

      continue;
    }

    if (strcmp(argv[0], "exit") == 0) {
      FREE_2D_ARRAY(argc, argv);
      free(back_check);
      back_check = (int*)NULL;
      break;
    }

    struct command_entry* comm_entry = fetch_command(argv[0]);


    if (comm_entry != NULL) { // pwd cd fg

      if(back==0){ // not background process
        int ret = comm_entry->command_func(argc, argv);
        if (ret != 0) {
          comm_entry->err(ret);
        }
      }
      else{ //if background process
      int status;
      if ((*back_check) == 1) {
              (*back_check) = 0;
              m = 0;
              local_pid = fork();

            if (local_pid == -1){
              printf("fork error\n");
              return 0;
            }

            else if(local_pid==0){ //child
            
             int pid2 = fork();

             if(pid2 > 0){ // parent
                FREE_2D_ARRAY(argc, argv);
                m = (*back_check);
                free(back_check);
                back_check = (int*)NULL;
                waitpid(pid2, NULL, 0);
                exit(0);
              }

              else if(pid2 == 0){ //child
              //printf("background process\n");
             
                int ret = comm_entry->command_func(argc, argv);
                if (ret != 0) {
                  comm_entry->err(ret);
               }
                m = (*back_check);
                free(back_check);
                back_check = (int*)NULL;
              
                FREE_2D_ARRAY(argc, argv);
                exit(0);
              }

              else{
                printf("fork error\n");
                FREE_2D_ARRAY(argc, argv);
                return 0;
              }
            }
            
            else{
              waitpid(WNOHANG, NULL, 0);
            }
      }
       else{

		   printf("running background process\n");
	   }
      }
    } else if (does_exefile_exists(argv[0])) {
      // TODO: Execute the program of argv[0]. 

      if(back ==0){
        int status;
        local_pid = fork();

        if (local_pid == -1){
          printf("fork error\n");
          return 0;
          }
        else if(local_pid==0){ 
          
          if(argv[0][0] != '/'){
            execvp(argv[0], argv);
          }

          else if(execv(argv[0],argv) <0)
                exit(-1);
          else
            exit(0);

        }
        
        else{ //parent
            wait(&status);
        }
      }
      else{ // background
         int status;
         if((*back_check) == 1){
          (*back_check) = 0;
          m = 0;
          local_pid = fork();

          if (local_pid == -1){ //fork error
            printf("fork error\n");
            return 0;
            }

          else if(local_pid==0){ // child
            int pid2 = fork();
            
            if(pid2 > 0){
              FREE_2D_ARRAY(argc, argv);
               m = (*back_check);
              free(back_check);
              back_check = (int*)NULL;
              waitpid(pid2, NULL, 0);
              exit(0);
            }
            else if(pid2==0){ 
             //printf("background process\n");
              
            if(argv[0][0] != '/'){
              m = (*back_check);
              free(back_check);
              back_check = (int*)NULL;
              execvp(argv[0], argv);
              }
            else if(execv(argv[0],argv) <0)
              exit(-1);
            else
              exit(0);
            }
            else{
                printf("fork error\n");
                FREE_2D_ARRAY(argc, argv);
                return 0;
              }
          }
          else{ //parent
            waitpid(WNOHANG, NULL, 0);
          }
        }
        else{
          
          printf("running background process\n");
        }
      }
    
    } 
    else {
      assert(comm_entry == NULL);
      fprintf(stderr, "%s: command not found.\n", argv[0]);
    }
    m = (*back_check);
    free(back_check);
    back_check = (int*)NULL;
    FREE_2D_ARRAY(argc, argv);
  }
  return 0;
}
