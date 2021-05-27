
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

#define TRUE 1
#define FALSE 0

#ifndef DEBUG
#define DEBUG 0
#endif
// #define DEBUG_PRINT(x) printf x
//f:debug print
void dprint(char * debugprint);

int dispatcher(int fd){//TODO controllare se va bene cosi,nella prova ho fatto un assegnamento ad una vatiabile usiliaria
  int fd_post;
  char  buff[256];
  while (1){//TODO GESTIONE DEI SEGNALI
  printf("aspettando una connessione");
  fd_post=m_accept(fd,NULL,0);
  //TODO HANDLER DELLA RICHIESTA DA PARTE DEL CLIENT
  }
  return 0;
}

int main(int argc, char const *argv[]) {
  int s_fd,s_fdpost;//fd per socket
  int error;

  pthread_t d_tid;

  initconfig( argc, argv[1]);//configurazione file conf
  printfconf();
  
  // creazione e inizializzazione della socket
  s_fd=m_socket(AF_UNIX,SOCK_STREAM,0);
  m_bind(s_fd,global.socketname,AF_UNIX);
  m_listen(s_fd,SOMAXCONN);
  s_fdpost=m_accept(s_fd,NULL,0);


  // TODO CONTROLLARE ARGOMENTI FUNZIONE PRIMA DI PANICARE
  if( (error=pthread_create(&d_tid,NULL, &dispatcher,s_fd)) !=0){
    errno=error;
    perror("\nCreating dispatcher thread: ");
    _exit(EXIT_FAILURE);
  }




  
  if( (error=pthread_join(d_tid,NULL)) !=0){
    errno=error;
    perror("\nJoining dispatcher thread: ");
    _exit(EXIT_FAILURE);
  }
  // chiusura
  close(s_fd);
  close(s_fdpost);
  unlink(global.socketname);
  exit(EXIT_SUCCESS);
  return 0;
}

void dprint(char * debugprint){
  if(DEBUG){
    printf("%s\n",debugprint);
    fflush(stdout);
  }
}
