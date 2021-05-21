
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<pthread.h>
#include<limits.h>
#include<stdint.h>
#include<signal.h>
#include<errno.h>
#include<unistd.h>
#include"./include/util.h"

#define TRUE 1
#define FALSE 0
#ifndef DEBUG
#define DEBUG 0
#endif
#define DEBUG_PRINT(x) printf x
//f:debug print
void dprint(char * debugprint);


int main(int argc, char const *argv[]) {

  initconfig( argc, argv[1]);
  printfconf();
  return 0;
}

void dprint(char * debugprint){
  if(DEBUG){DEBUG_PRINT((debugprint));fflush(stdout);}
}
