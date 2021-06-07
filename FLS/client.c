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
#include <fcntl.h>
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

list_t* msglist;
int ttwait;//tempo di attesa per invio delle richieste

void handle_h(){
  printf("opzioni possibili:\n-f,-w,-W,-r,-R,-d,-t,-c,-p\n");
}

void handle_t(char* args){
  ttwait=atoi(args);
  if(DEBUG){printf("ttwait settato a: %d\n",ttwait);}
}

void handle_f(){
  printf("SOCKETNAME: %s\n",SOCKNAME);
}

void handle_d(char*path){
  struct stat st = {0};
  if (stat(path, &st) == -1) {
    mkdir(path,0777);
  }else printf("cartella già esistente\n");
}

void handle_r(msg message){
  printf("qui\n");

  char args[MAXSTRLEN];
  char dest[MAXSTRLEN];
  strcpy(args,message.args);
  strcpy(dest,message.dest);
  int n_args=countOccurrences(args,",");
  n_args++;
  printf("args ricevuto %s , n_args %d\n",args,n_args);
  
  if(n_args>1){//se ho più di un argomento inserisco i nomi dei file in un array di stringhe,per gestire separatamente le richieste
    int i=0;
    int err=0;
    char filenames[n_args][MAXSTRLEN];
    printf("numero di argomenti :%d\n",n_args);
    char * token=strtok(args,",");
    while(token!=NULL){
      strcpy(filenames[i],token);
      printf("filenames %s token %s\n",filenames[i],token);
      token = strtok(NULL, ",");
      i++;
    }
    for(int j=0;j<n_args;j++){
      int err =openFile(filenames[j],O_CREAT);
      if(err<0){
        err=openFile(filenames[j],NULL);
      }//se ancora negativo interrompi

    }
  }
  else{
    printf("ramo else di handle_r\n");
    int err =openFile(args,O_CREAT);
    char buffer[MAXSTRLEN];
    /*gestione dell'errore err*/
    //int err_r=readFile(const char *pathname, void **buf, size_t *size);
  }
}

int handle_message(msg message){
  printf("messaggio ricevuto in handle_message %c %s\n",message.op,message.args);
  char opt=message.op;
  char args[MAXSTRLEN];
  strcpy(args,message.args);
  printf("dopo copia %s\n",args);
  switch(opt){
    case 'h':
      handle_h();
      return EXITNOW;
      break;
    case 't':
      handle_t(args);
      break;
    case 'f':
      handle_f();
      return EXITNOW;
      break;
    case 'd':
      handle_d(message.args);
      break;
    case 'r':
      handle_r(message);
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

    fgets(buffer, MAXSTRLEN,stdin);    //leggo dallo stdin la richiesta
    ptropt=strchr(buffer,'-');//cerco la prima occorrenza di "-"
    if(ptropt==NULL){//mi assicuro che il formato della richiesta sia sia corretto
      perror("Formato Input sbagliato");
      //exit(EXIT_FAILURE);
      return msglist;
    }
    *ptropt++;//prendo la lettera dopo
    message.op=*ptropt;

    if((ptropt2=strchr(ptropt,'-'))!=NULL){//se ho un secondo comando
      *ptropt2++;
      message2.op=(*ptropt2);
      ptrarg2=strchr(ptropt2,' ');
      strcpy(message2.args,ptrarg2);//TODO AGGIUNGERE SPAZIETTO POST 
      printf("message2 : op %c arg %s\n",message2.op,message2.args);
    }

    ptrarg=strtok(ptropt,"-");//cerco la stringa degli argomenti dopo lo spazio
    *ptrarg++;
    strcpy(message.args,ptrarg);
    if(DEBUG){printf("message: op %c arg %s\n",message.op,message.args);}

    if( ( (message.op=='d')|| (message2.op=='d') ) && ( ((message.op!='r') && (message.op!='R')) ) ) {
      printf("message2: op%cmessage1: op%cxcxg\n",message2.op,message.op);
      printf("formattazzione della richiesta errata\n");
     // return;
    }


    if((message2.op=='d') && ((message.op=='r') || (message.op=='R'))){
      printf("sto per gestire %c %s\n",message2.op,message2.args);
      if((err=handle_message(message2))==EXITNOW){
       // return;
      }
      strcpy(message.dest,message2.args);
      printf("sto per gestire %c %s con destinazione %s\n",message.op,message.args,message.dest);
      if((err=handle_message(message))==EXITNOW){
      //return;
      }
    }
    else{
      //TODO implementare sleep
      if((err=handle_message(message))==EXITNOW){
      //return;
      }
    
      if((err=handle_message(message2))==EXITNOW){
      //return;
      }
    }
    //tail_insert(msglist,message);
  }while((ptropt=strchr(buffer,'-'))!=NULL);
  
  return msglist;
}

int main(int argc, char const *argv[]) {
  msglist=new_list();
  char messaggio[257]="wewe sono il client";
  double msec=200.0;
  printf("double msec %lf\n",msec);
  struct timespec maxtemp;
  maxtemp.tv_sec=3;
  openConnection(SOCKNAME,msec,maxtemp);
  while(1){//TODO GESTIONE DEI SEGNALI 
      msg message;
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
