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

#ifndef DEBUG
#define DEBUG 0
#endif
//struttura dati per la configurazione iniziale
typedef struct conf{
  int nworkers;//numero di thread workers
  int dim;//dimensione dello spazio di memorizzazione
  char*socketname;//nome della socket
}conf;

conf global;

//configurazione dal file di testo config.txt
void initconfig(int argc,char const * pos){
  if(argc!=2){
    printf("input file mancante\n");
    exit(EXIT_FAILURE);
  }
  if(strcmp("config.txt",pos)!=0){
    printf("file config.txt non trovato\n");
    exit(EXIT_FAILURE);
  }
  int numworkers;
  int dim;
  char socketname[20]={0};

  FILE*fd;
  if((fd=fopen(pos,"r"))==NULL){
    perror("aprendo il file");
    fclose(fd);
    exit(EXIT_FAILURE);
  }
  fscanf(fd,"%d;%d;%[^;]",&numworkers,&dim,socketname);//[^;]==tutti i valori eccetto ;
  global.nworkers=numworkers;
  global.dim=dim;
  global.socketname=socketname;
  fclose(fd);
}

void printfconf(){
  printf("CONFIGURATION FILE:\nsocketname: %s\nnumworkers: %d\nspazio_mem.: %d\n",global.socketname,global.nworkers,global.dim);
}
//Jacob Sorber TM
/*void err_n_die(const char * fmt, ...){
  int errno_save;
  va_list ap;
  //salviamo errno settato dalle SC
  errno_save=errno;
  //print di fmt+args nello stdout
  va_start(ap,fmt);
  vfprint(stdout,fmt,ap);
  fprintf(stdout, "\n" );
  fflush(stdout);
  //printa messaggio errore
  if(errno_save !=0){
    fprintf(stdout, "errno= %d  : %s\n", errno_save,sterror(errno_save));
    fprintf(stdout, "\n");
    fflush(stdout);
  }
  va_end(ap);
  exit(1);
}

*/

int m_socket(int domain, int type, int protocol){
  int fd;
  if( (fd=(socket(domain,type,protocol))) ==-1){
    perror("creazione socket");
    exit(EXIT_FAILURE);
  }
  return fd;
}

void m_bind(int sockfd,char * sname,int sfamily){
  struct sockaddr_un sa;
  strcpy(sa.sun_path,sname);
  sa.sun_family=sfamily;
  if( (bind(sockfd,(struct sockaddr*)&sa,sizeof(sa))) ==-1){
    perror("bind socket");
    exit(EXIT_FAILURE);
  }
}
void m_listen(int sockfd, int backlog){
  int fd;
  if( (fd=(listen(sockfd,backlog))) ==-1){
    perror("listen socket");
    exit(EXIT_FAILURE);
  }
}

int m_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
  int newfd;
  newfd=accept(sockfd,addr,addrlen);
  return newfd;
}



void m_connect(int sockfd,char* sname,int sfamily){
  struct sockaddr_un sa;
  strcpy(sa.sun_path,sname);
  sa.sun_family=sfamily;
  while( (connect(sockfd,(struct sockaddr*)&sa,sizeof(sa))) ==-1){
    if(errno==ENOENT) sleep(1);
    else exit(EXIT_FAILURE);
  }
}


void m_lock(pthread_mutex_t* mtx) {
  int error;
  if( (error=pthread_mutex_lock(mtx)) !=0) {
    errno=error;
    perror("\nLocking mutex: ");
    _exit(EXIT_FAILURE);
  }
  if(DEBUG){printf("Mutex locked\n");}
  return;
}

void m_unlock(pthread_mutex_t* mtx) {
  int error;
  if( (error=pthread_mutex_unlock(mtx))!=0) {
    errno=error;
    perror("\nUnlocking mutex: ");
    _exit(EXIT_FAILURE);
  }
  if(DEBUG){printf("Mutex unlocked\n");}
  return;
}

void m_wait(pthread_cond_t* cond, pthread_mutex_t* mtx) {
  int error;
  if((error=pthread_cond_wait(cond, mtx))!=0) {
    errno=error;
    perror("\nPreparing to wait for cond: ");
    _exit(EXIT_FAILURE);
  }
  if(DEBUG){printf("Wait in corso\n");}

  return;
}

void m_signal(pthread_cond_t* cond) {
  int error;
  if((error=pthread_cond_signal(cond))!=0) {
    errno=error;
    perror("\nSignaling cond: ");
    _exit(EXIT_FAILURE);
  }
  if(DEBUG){printf("Signal inviata\n");}
  return;
}

int countOccurrences(char * str, char * toSearch){
    int i, j, found, count;
    int stringLen, searchLen;
    stringLen = strlen(str);      // length of string
    searchLen = strlen(toSearch); // length of word to be searched
    count = 0;
    for(i=0; i <= stringLen-searchLen; i++){
        //Match word with string 
        found = 1;
        for(j=0; j<searchLen; j++){
            if(str[i + j] != toSearch[j]){
                found = 0;
                break;
            }
        }
        if(found == 1){
            count++;
        }
    }
    return count;
}
ssize_t readn(int fd, void *ptr, size_t n) {  /* Read "n" bytes from a descriptor */
   size_t   nleft;
   ssize_t  nread;
 
   nleft = n;
   while (nleft > 0) {
     if((nread = read(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1; /* error, return -1 */
        else break; /* error, return amount read so far */
     } else if (nread == 0) break; /* EOF */
     nleft -= nread;
     ptr   += nread;
   }
   return(n - nleft); /* return >= 0 */
}
 
ssize_t writen(int fd, void *ptr, size_t n) {  /* Write "n" bytes to a descriptor */
   size_t   nleft;
   ssize_t  nwritten;
 
   nleft = n;
   while (nleft > 0) {
     if((nwritten = write(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1; /* error, return -1 */
        else break; /* error, return amount written so far */
     } else if (nwritten == 0) break; 
     nleft -= nwritten;
     ptr   += nwritten;
   }
   return(n - nleft); /* return >= 0 */
}