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
#include<sys/param.h>
#include <fcntl.h>
#include"./include/util.h"
#include"./include/List/linked_list.h"
#include"./include/List/queue.h"
#include"./include/chained_hash.h"//TODO RICORDA DI CAMBIARE IL MAKEFILE E COMPILARE CON -LM E COMPILARE CHAINED HASH.C

#define TRUE 1
#define FALSE 0
//#define MAXFILESIZE 4096
#define MAXPATH 1000

#ifndef DEBUG
#define DEBUG 0
#endif
//PER LA POLITICA DI RIMPIAZZAMENTO POSSO FARE CHE CERCO TRA I PRIMI ELEMENTI DI OGNI LISTA CHI HA ORDER MINORE
char*toreadpath="SERVER/";
int capacity=0;
int numfiles=0;//TODO fare mutex  per ogni variabile globale
int order=0;
chained_hash_t* f_hasht;

pthread_mutex_t ordermutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hashmutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t capacitymutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexqueue=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvqueue=PTHREAD_COND_INITIALIZER;

void printmsg(msg request){
  printf("Message :\nOP: %c\nARG:%s\nDEST: %s\nFLAG:%d\n",request.op,request.args,request.dest,request.flag);
}

msg copymsg(msg to_copy){
  msg to_ret;
  to_ret.op=to_copy.op;
  strcpy(to_ret.args,to_copy.args);
  strcpy(to_ret.dest,to_copy.dest);
  to_ret.flag=to_copy.flag;
  return to_ret;
}


void handle_o(msg request,int client_socket){//TODO IMPLEMENTARE RIMOZIONE FILE SE ECCESSO
  //provo a calcolare il path
  char tosearch[MAXPATH];
  strcpy(tosearch,toreadpath);
  strcat(tosearch,request.args);
  printf("tosearch %s\n",tosearch);
  file_s file_to_s={0};
  int found=chained_hash_search(f_hasht,8,&file_to_s,tosearch);
  if(DEBUG) printf("file trovato ? : %d\n",found);
  if(found==0 && request.flag==O_CREAT){//se il file non esiste e ho il flag per crearlo
    //creo il path
    char nullpath[MAXPATHLEN];
    memset(nullpath, 0, sizeof(nullpath));
    printf("nullpath1:%s\n",nullpath);
    strcat(nullpath,toreadpath);
    printf("nullpath2:%s\n",nullpath);
    strcat(nullpath,request.args);
    printf("nullpath3:%s\n",nullpath);
    if(DEBUG){printf("HANDLE_O: CREAZIONE FILE IN PATH %s\n",nullpath);}
    //creo il file e lo apro in lettura e scrittura(la dicitura "b" è per aprire eventualmente file non strettamente di testo)
    int err =0;
    FILE* fileto_o=fopen(nullpath,"wb+");
    if(fileto_o==NULL){//se non sono riuscito a crearlo
      err=-1;
      writen(client_socket,&err,sizeof(err));
      printf("file %s NON creato ERRORE\n",request.args);
    }
    else{//ho creato il file
      //inizializzo la struct file_s che rappresenterà il file nella hashtable
      file_s* newfile=malloc(sizeof(file_s));
      newfile->fp=fileto_o;
      m_lock(&ordermutex);
      newfile->order=order;
      order++;
      m_unlock(&ordermutex);
      strcpy(newfile->fname,nullpath);//TODO CONTROLLARE SE LUNHGEZZA FNAME VA BENE
      printf("NULLPATH: %s\n",nullpath);
      //inserisco nella hashtable
      m_lock(&hashmutex);
      chained_hash_insert(f_hasht,8,*newfile);//printhash(f_hasht);
      m_unlock(&hashmutex);
      //restituisco il risultato dell'operazione al client
      writen(client_socket,&err,sizeof(err));
      if(DEBUG){printf("file %s creato\n",request.args);}
      return;
    }
  }
  else if(found==1 && request.flag!=O_CREAT){//Se il file esiste già e il flag è,correttamente,diverso da O_CREAT 
    int err =0;
    //file_s file_to_s={0};//
    //file_s* file_to_s=malloc(sizeof(file_s));
    //if(DEBUG){printf("path: %s\n",path);}
    m_lock(&hashmutex);
   // chained_hash_search(f_hasht,8,&file_to_s,path);
    m_unlock(&hashmutex);
    int isopen=ftell(file_to_s.fp);//Controllo se il file è aperto //TODO sta cosa funziona,forse,per non so quale miracolo in teoria funziona con un FILE *
    if(isopen==-1){//se è chiuso lo apro
      FILE *fileto_o=fopen(tosearch,"w+");
      if(fileto_o==NULL){//se non riesco ad aprirlo lo segnalo
        err=-1;
        writen(client_socket,&err,sizeof(err));
        printf("file %s NON aperto ERRORE\n",request.args);
      }
      else{//altrimenti segnalo che tutto è andato bene
        
        writen(client_socket,&err,sizeof(err));
        printf("file %s aperto\n",request.args);
      }
    }
    else{//se è aperto 
      printf("file already opened.\n");//TODO segnalare che tutto ok al client
      writen(client_socket,&err,sizeof(err));
      printf("segnale inviato al client,file gia aperto\n");
    }
  }
  else if(found==1 && request.flag==O_CREAT){//se il file esiste già ma il flag è sbagliato lo segnalo al client 
    int err=-2;
    writen(client_socket,&err,sizeof(err));
    printf("file già esistente,flag errato\n");
  }
}

void handle_r(msg request,int client_socket){

  //file_s * file_to_r=malloc(sizeof(file_s));//TODO vedere se funziona anche se non alloco la memoria dinamicamente,in caso non possa non dichiararlo dinamicamente bisogna effettuare la free
  file_s file_to_r={0};
  rep reply;

  //creo il path per il file da cercare
  char provapath[MAXPATH];//TODO controllare se è sufficiente come dimensione
  memset(provapath, 0, sizeof(provapath));
  strcat(provapath,toreadpath);//TODO resettare la stringa guarda il terminale ->
  strcat(provapath,request.args);
  printf("provapath: %s\n",provapath);

  m_lock(&hashmutex);
  chained_hash_search(f_hasht,8,&file_to_r,provapath);
  m_unlock(&hashmutex);
  //TODO una volta ottenuto il file scrivo,temporaneamente (TODO cancellare la scrittura),lo leggo nella sua interezza
  char buff[123]= "w powerpizza";
  fprintf(file_to_r.fp,"%s",buff);//fwrite(buff,1,123,file_to_r->fp);
  printf("dopo fprintf\n");
  fseek(file_to_r.fp,0,SEEK_SET);
  printf("dopo fseek\n");
  fread(reply.args,MAXFILESIZE,1,file_to_r.fp); 
  printf("dopo fread\n");

  if(strlen(reply.args)>0){//se il file non è vuoto
    printf("sono dentro if\n");
    reply.err =0;
    writen(client_socket,&reply,sizeof(reply));//e lo mando al client
  }
  else{//altrimenti se il file è vuoto
    reply.err =-1;
    writen(client_socket,&reply,sizeof(rep));
  }
}

void handle_x(int client_socket){
  printf("client n: %d ha terminato \n",client_socket);
}

int handleoperation(msg request,int client_socket){
  char op=request.op;
  printf("dentro handle operation %c %s\n",op,request.args);
  switch(op){
    case 'o':
      handle_o(request,client_socket);
      break;
    case 'r':
      handle_r(request,client_socket);
      break;
    case 'x':
      handle_x(client_socket);
      break;
  }return 0;
}
void * handleconnection(void * arg){
   int client_socket=*((int*)arg);
   int twop=0;
   free(arg);//free effettuata subito perchè arg non mi serve più //char  buffer[1024];
   int exit =0;
   do{//TODO PROVVISORIO OVVIAMENTE NON VA BENE
     printf("sono dentro il while della morte\n");
     msg request={0};
     readn(client_socket,&request,sizeof(request));
     printmsg(request);
     if(request.op=='x'){
       exit=1;
     }
     if(request.op=='o'){
      twop=1;
      printf("twop: %d\n",twop);
     }
     // printf("ricevuta richiesta da:%d,ho letto: %s\n",client_socket,buffer);
     handleoperation(request,client_socket);
     twop--;
     printf("dopo handleoperation,twop: %d\n",twop);
   }while(/*twop>=0 &&*/ exit!=1);
   printf("sono uscito dal while\n");
   return 0;
}


void * threadfunction(void * arg){
  while(TRUE){//TODO vedere quando termina
    int * fd_client;

    m_lock(&mutexqueue);//effettuo la lock sulla coda
    if( (fd_client=dequeue())==NULL){//se la coda è vuota 
      m_wait(&condvqueue,&mutexqueue);//aspetto una signal che mi dica che esiste almeno un elemento in coda
      fd_client=dequeue(); //e riprovo una volta piena
    }
    m_unlock(&mutexqueue);//faccio la unlock
    if(fd_client!=NULL){//se quindi esiste una connessione con un client pendente
      handleconnection(fd_client);//mi prendo cura della richiesta
    }
  }
}


void* dispatcher(void * p_fd){//TODO controllare se va bene cosi,nella prova ho fatto un assegnamento ad una vatiabile usiliaria
  int fd=*((int*)p_fd);
  int fd_post;

  pthread_t thread_pool[global.nworkers]; //creo la pool di thread 
  for (int i = 0; i < global.nworkers; i++){//assegno la funzione di accettazione
    pthread_create(&thread_pool[i],NULL,threadfunction,NULL);
  }

  while(TRUE){//TODO GESTIONE DEI SEGNALI
    if(DEBUG){printf("aspettando una connessione...\n");}
    fd_post=m_accept(fd,NULL,0);//aspetto la connessione da parte dei client
    if(DEBUG){ printf("connessione avvenuta con %d\n",fd_post);}

    int *fd_client=malloc(sizeof(int));//alloco un puntatore a intero cosi mi viene meglio a passarlo tra thread e funzioni
    *fd_client=fd_post;//gli assegno il file descriptor del client da gestire
    
    m_lock(&mutexqueue);
    enqueue(fd_client);//e lo metto in coda
    m_signal(&condvqueue);//segnalando che la coda ha ora almeno un elemento
    m_unlock(&mutexqueue); 
  }
    return 0;

}
int main(int argc, char const *argv[]) {
  int s_fd;
  int error;
  int m=10;//numero di posizioni nella tabella hash
  pthread_t d_tid;//thread id per il dispatcher
  f_hasht=new_hash_table(m);
  initconfig( argc, argv[1]);//configurazione file conf
  capacity=global.dim;
  printfconf();

  // creazione e inizializzazione della socket
  s_fd=m_socket(AF_UNIX,SOCK_STREAM,0);
  m_bind(s_fd,global.socketname,AF_UNIX);
  m_listen(s_fd,SOMAXCONN);

  //l'accettazione dei client viene effettutata da un thread separato,il dispatcher.
  if( (error=pthread_create(&d_tid,NULL, &dispatcher,&s_fd)) !=0){
    errno=error;
    perror("\nCreating dispatcher thread: ");
    _exit(EXIT_FAILURE);
  }

  // chiusura
  if( (error=pthread_join(d_tid,NULL)) !=0){
    errno=error;
    perror("\nJoining dispatcher thread: ");
    _exit(EXIT_FAILURE);
  }
  close(s_fd);
  unlink(global.socketname);
  exit(EXIT_SUCCESS);
  return 0;
}

