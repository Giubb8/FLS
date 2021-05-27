
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
#include<stdarg.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include"./include/util.h"

MAXSTRLEN  1024

typedef struct message{
  char op;
  char args[MAXSTRLEN];
}msg;


msg parsemsg(){
  msg message;
  char buffer[MAXSTRLEN];
  char * ptropt;
  char * ptrarg;
  //leggo dallo stdin la richiesta
  fgets(buffer, MAXSTRLEN,stdin);
  ptropt=strchr(buffer,'-');//cerco la prima occorrenza di "-"
  if(ptropt==NULL){//mi assicuro che il formato della richiesta sia sia corretto
    perror("Formato Input sbagliato");
    exit(EXIT_FAILURE);
  }
  *ptropt++;//prendo la lettera dopo
  ptrarg=strchr(buffer,' ');//cerco la stringa degli argomenti dopo lo spazio 
  message.op=*ptropt;
  strcpy(message.args,ptrarg);
  if(DEBUG){printf("message: %c %s",message.op,message.args);}
  return message;
}


int main(int argc, char const *argv[]) {
  int fd;
  initconfig( argc, argv[1]);//configurazione file conf
  printfconf();


  fd=socket(AF_UNIX,SOCK_STREAM,0);
  m_connect(fd,global.socketname,AF_UNIX);
  while(1){//TODO GESTIONE DEI SEGNALI 
    msg message;
    message=parsemsg();//TODO non mi ricordo se posso copiare una struct cosi
    fscanf(stdin,"-c");
    //impacchetta
    //spedisce
  }

  close(fd);
  exit(EXIT_SUCCESS);

  return 0;
}
