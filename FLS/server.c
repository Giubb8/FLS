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
#include"./include/List/queue.h"
#define TRUE 1
#define FALSE 0

#ifndef DEBUG
#define DEBUG 0
#endif
pthread_mutex_t mutexqueue=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvqueue=PTHREAD_COND_INITIALIZER;


void * handleconnection(void * arg){
   int client_socket=*((int*)arg);
   free(arg);//free effettuata subito perchè arg non mi serve più
   char  buffer[1024];
   read(client_socket,buffer,sizeof(buffer));
   printf("ricevuta richiesta da:%d,ho letto: %s\n",client_socket,buffer);
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
  pthread_t d_tid;//thread id per il dispatcher

  initconfig( argc, argv[1]);//configurazione file conf
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

