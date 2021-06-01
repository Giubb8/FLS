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
#include <sys/stat.h>
#include"./include/util.h"
#include"./include/List/linked_list.h"
#include"./include/api.h"

#ifndef DEBUG
#define DEBUG 0
#endif
#define EXITNOW -8
#define MAXSTRLEN 1024
#define SOCKNAME "./fls"
/*
typedef struct message{
  char op;
  char args[MAXSTRLEN];
}msg;
*/
list_t* msglist;
int ttwait;



/*
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
  message.op=*ptropt;
  ptrarg=strchr(buffer,' ');//cerco la stringa degli argomenti dopo lo spazio 
  strcpy(message.args,ptrarg);
  if(DEBUG){printf("message: %c %s",message.op,message.args);}
  return message;
}
*//*
list_t* parsemsg(){
  char buffer[MAXSTRLEN];
  char * ptropt;
  char * ptrarg;
  do{
    msg message;
    //leggo dallo stdin la richiesta
    fgets(buffer, MAXSTRLEN,stdin);
    ptropt=strchr(buffer,'-');//cerco la prima occorrenza di "-"
    if(ptropt==NULL){//mi assicuro che il formato della richiesta sia sia corretto
      perror("Formato Input sbagliato");
      //exit(EXIT_FAILURE);
      return msglist;
    }
    *ptropt++;//prendo la lettera dopo
    message.op=*ptropt;
    ptrarg=strchr(buffer,' ');//cerco la stringa degli argomenti dopo lo spazio
    *ptrarg++;
    strcpy(message.args,ptrarg);
    tail_insert(msglist,message);
    //if(DEBUG){printf("message: %c %s",message.op,message.args);}
  }while((ptropt=strchr(buffer,'-'))!=NULL);
  
  return msglist;
}
*/
void handle_h(){
  printf("opzioni possibili:\n-f,-w,-W,-r,-R,-d,-t,-c,-p\n");
  
}
void handle_t(char* args){
  ttwait=atoi(args);
  if(DEBUG){printf("ttwait settato a: %d\n",ttwait);}
}

void handle_f(){
  printf("SOCKETNAME: %s",SOCKNAME);
}

void handle_d(char*path){
  struct stat st = {0};
  if (stat(path, &st) == -1) {
    mkdir(path,0777);
  }else printf("cartella già esistente");

}
int handle_message(msg message){
  char opt=message.op;
  switch(opt){
    case 'h':
      handle_h();
      return EXITNOW;
      break;
    case 't':
      handle_t(message.args);
      break;
    case 'f':
      handle_f();
      return EXITNOW;
      break;
    case 'd':
      handle_d(message.args);
      break;
  }return 0;

}

list_t* parsemsg(){
  int err;
  char buffer[MAXSTRLEN];
  char * ptropt;
  char * ptropt2;
  char * ptrarg;
  char * ptrarg2;
  do{
    msg message;
    msg message2;
    //leggo dallo stdin la richiesta
    fgets(buffer, MAXSTRLEN,stdin);
    ptropt=strchr(buffer,'-');//cerco la prima occorrenza di "-"
    if(ptropt==NULL){//mi assicuro che il formato della richiesta sia sia corretto
      perror("Formato Input sbagliato");
      //exit(EXIT_FAILURE);
      return msglist;
    }
    *ptropt++;//prendo la lettera dopo
    message.op=*ptropt;
    if((ptropt2=strchr(ptropt,'-'))!=NULL){
      *ptropt2++;
      message2.op=(*ptropt2);
      ptrarg2=strchr(ptropt2,' ');
      strcpy(message2.args,ptrarg2);
      printf("message2 : op %c arg %s\n",message2.op,message2.args);
    }
    ptrarg=strtok(ptropt,"-");//cerco la stringa degli argomenti dopo lo spazio
    *ptrarg++;
    strcpy(message.args,ptrarg);
    if(DEBUG){printf("message: op %c arg %s\n",message.op,message.args);}

    if((message.op=='d')||( (message2.op=='d') && ( ((message.op!='r') && (message.op!='R')) ) ) ){
      printf("message2: op%cmessage1: op%cxcxg\n",message2.op,message.op);
      printf("formattazzione della richiesta errata\n");
      return;
    }
    else{
      if((err=handle_message(message))==EXITNOW){
      return;
    }
    }
    if((err=handle_message(message2))==EXITNOW){
      return;
    }
    //tail_insert(msglist,message);
  }while((ptropt=strchr(buffer,'-'))!=NULL);
  
  return msglist;
}


int main(int argc, char const *argv[]) {
  msglist=new_list();
  char messaggio[257]="wewe sono il client";
  double msec=200.0;
  printf("double msec %lf",msec);
  struct timespec maxtemp;
  maxtemp.tv_sec=3;
  openConnection(SOCKNAME,msec,maxtemp);
  /*fd=socket(AF_UNIX,SOCK_STREAM,0);
  m_connect(fd,SOCKNAME,AF_UNIX);*/
  //printf("connessione lato client avvenuta: il mio fd e' %d\n",fd);
  //write(fd,messaggio,sizeof(messaggio));
 
 while(1){//TODO GESTIONE DEI SEGNALI 
    msg message;//TODO IL PARSING POSSO FARLO NEL SERVER
    list_t* lista=new_list();
    parsemsg();//TODO non mi ricordo se posso copiare una struct cosi
    print_list(lista);
    //impacchetta
    //spedisce
  }

  close(fd);
  exit(EXIT_SUCCESS);

  return 0;
}
