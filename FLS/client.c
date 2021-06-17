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
#include<fcntl.h>
#include<sys/stat.h>
#include"./include/util.h"
#include"./include/List/linked_list.h"
#include"./include/api.h"
#include"./include/chained_hash.h"


#ifndef DEBUG
#define DEBUG 0
#endif

#define EXITNOW -8
#define MAXSTRLEN 1024
#define MAXFILESIZE 40960
#define MAXPATH 1000
#define SOCKNAME "./fls"
#define CLIENTDIR "CLIENT/"

chained_hash_t* fc_hasht;
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
  char npath[1000];
  createpath(&npath,CLIENTDIR,path);
  printf("path cartella calcolato: %s\n",npath);
  if (stat(npath, &st) == -1) {
    mkdir(npath,0777);
  }else printf("cartella già esistente\n");
}

void handle_r(msg message){
  char args[MAXSTRLEN];//nome del/dei file
  char dest[MAXSTRLEN];//eventuale destinazione
  strcpy(args,message.args);
  strcpy(dest,message.dest);
  int n_args=countOccurrences(args,",");
  n_args++;
  printf("args ricevuto %s , n_args %d, destinazione %s\n",args,n_args,dest);
  
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
      //apro il file j-esimo
      int err =openFile(filenames[j],O_CREAT);
      if(err<0){
        err=openFile(filenames[j],12);//TODO CAMBIARE PURE QUA IL FLAG IN QUALCOSA DI SENSATO
        if(err<0){
          continue;
        }
      }
      //preparazione per la lettura del file
      char buffer[MAXFILESIZE];
      int size=MAXFILESIZE;
      int err_r=readFile(filenames[j], (void**)&buffer,&size);//TODO GESTIONE DELL'ERRORE ERR
      if((strlen(message.dest)>0)){//se devo storare il file letto in una destinazione nota
        printf("STRLEN DI PATH MAGGIORE DI 0\n");
        file_s * newfile=malloc(sizeof(file_s));
        //creo il path nel quale andrà storato il file,TODO possibile soluzione elegante funzione ad argomenti variabili,ordine di inserimento uguale ordine di comparsa nel path
        char newpath[MAXPATH];
        strcpy(newpath,CLIENTDIR);
        strcat(newpath,dest);
        strcat(newpath,"/");
        strcat(newpath,filenames[j]);
        remove_spaces(newpath);
        printf("newpath calcolato %s\n",newpath);

        FILE* newf=fopen(newpath,"w+");
        fprintf(newf,"%s",buffer);//copio il contenuto del buffer nel server
        fclose(newf);
        newfile->fp=newf;
        strcpy(newfile->fname,filenames[j]);
        chained_hash_insert(fc_hasht,9,*newfile);
      }
      else{
        printf("LETTO DAL SERVER: %s\n",buffer);
      }
    }
  }
  else{//se ho un solo argomento della read
    printf("ramo else di handle_r\n");
    //cerco di aprire il file,se il flag O_CREAT non dovesse andare bene riprovo con un altro file
    int err =openFile(args,O_CREAT);
    if(err<0){
      err=openFile(args,12);//TODO cambiare il flag in qualcosa di sensato
      if(err<0){
        perror("openFile");
        exit(EXIT_FAILURE);      
      }
    }

    char buffer[MAXFILESIZE];
    int size=MAXFILESIZE;
    int err_r=readFile(args, (void**)&buffer,&size);    /*gestione dell'errore err*/
    if((strlen(message.dest)>0)){
      printf("CLIENT,DESTINAZIONE DIVERSA DA NULL\n");
      file_s * newfile=malloc(sizeof(file_s));
      char newpath[MAXPATH];
      strcpy(newpath,CLIENTDIR);
      strcat(newpath,dest);
      strcat(newpath,"/");
      strcat(newpath,args);
      remove_spaces(newpath);
      printf("DESTINAZIONE FILE:%s\n",newpath);
      FILE* newf=fopen(newpath,"w+");
      newfile->fp=newf;
      fprintf(newf,"%s",buffer);
      fclose(newf);
      strcpy(newfile->fname,args);
      chained_hash_insert(fc_hasht,9,*newfile);
    }
    else{
      printf("LETTO DAL SERVER: %s\n",buffer);
    }
  }
}
void handle_x(){
  msg message;
  message.op='x';
  int w=writen(fd,&message,sizeof(message));//TODO GESTIONE ERRORE DA PARTE DEL SERVER
  if(DEBUG){printf("operation x,writen : %d, il client ha terminato le sue richieste.\n",w);}
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
    case 'x':
      handle_x();
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
    msg message={0};
    msg message2={0};

    fgets(buffer, MAXSTRLEN,stdin);    //leggo dallo stdin la richiesta
    buffer[strcspn(buffer, "\n")] = 0;//serve per cancellare lo \n che tipicamente mette la fgets
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
      *ptropt2++;
      *ptropt2++;

      strcpy(message2.args,ptrarg2);//TODO AGGIUNGERE SPAZIETTO POST 
      printf("message2 : op %c arg %s\n",message2.op,message2.args);
    }

    ptrarg=strtok(ptropt,"-");//cerco la stringa degli argomenti dopo lo spazio
    *ptrarg++;
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
  
  }while((ptropt=strchr(buffer,'-'))!=NULL);
  
  return msglist;
}

int main(int argc, char const *argv[]) {
  int m=10;
  msglist=new_list();
  fc_hasht=new_hash_table(m);
  double msec=200.0;//printf("double msec %lf\n",msec);
  struct timespec maxtemp;
  maxtemp.tv_sec=3;
  openConnection(SOCKNAME,msec,maxtemp);
  while(1){//TODO GESTIONE DEI SEGNALI 
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
