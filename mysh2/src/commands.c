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
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <utils.h>
#include "fs.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


#define SIZE 128


int kill(pid_t pid, int signo);

static struct command_entry commands[] =
{
  {
    "pwd",
    do_pwd,
    err_pwd
  },
  {
    "cd",
    do_cd,
    err_cd
  }
  ,
  {
    "fg",
    do_fg,
    err_fg
  }
};

struct command_entry* fetch_command(const char* command_name)
{
  // TODO: Fill it.
  struct command_entry* temp = commands;
  if(strcmp(command_name, "pwd")==0){
    return temp;
  }
  else if(strcmp(command_name,"cd")==0){
    return temp+1;
  }else if(strcmp(command_name,"fg")==0){
    return temp+2;
  }
  else{
    return NULL;
  }
}

int do_pwd(int argc, char** argv)
{
  // TODO: Fill it.
  if(argc!=1){
    printf("not command\n");
    return 1;
  }
    char buff[SIZE];
    getcwd(buff,SIZE);
    printf("%s\n", buff);
    fflush(stdout);
    return 0;
}

void err_pwd(int err_code)
{
  // TODO: Fill it.
  // no fill it.
}

int do_cd(int argc, char** argv)
{
  // TODO: Fill it.
  if(argc<2) return 0;

  if(does_exefile_exists(argv[1])==0){
    return 1;
  }
  else if(argc == 2){
    if(chdir(argv[1])){
      return 2;
    }
  }
  else{
    return 0;
  }
}

void err_cd(int err_code)
{
  // TODO: Fill it.
  if(err_code==1) {
    fprintf(stderr, "cd: no such file or directory\n");
  }
  if(err_code==2) {
    fprintf(stderr, "cd: not a directory\n");
  }
}

void signal_handler(int signo){
  switch(signo){
    case SIGINT:
      signal(SIGINT, SIG_IGN);
      break;
    case SIGTSTP:
      signal(SIGTSTP, SIG_IGN);
      break;
    case SIGCHLD:
    {
      pid_t pid;
      int stat ;
      
      while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
      {
        (*back_check) =1;
        m = 1;
      fprintf(stderr,"child %d terminated normaly", pid);
      }
      break;
    }
   }
}

void signal_generator(int pid){
   kill(pid, SIGKILL);
   fflush(stdout);
 }


void err_fg(int err_code){ //
  if(err_code ==1) {
    printf("incorrected command\n");
    return;
  }
  else if(err_code ==2){
    printf("fg: current: no such job\n");
    return;
  }
}

int do_fg(int argc, char** argv){
  if(argc !=1){
   return 1;
  }
  else if((*back_check) == 1){
    return 2;
  }

  else{ 
    (*back_check) =1;
      m = 1;
    printf("background -> foreground process!!!\n");
    waitpid(local_pid,0,0);   
    return 0;
  }

}