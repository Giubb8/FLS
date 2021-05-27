
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
  if(error=pthread_mutex_lock(mtx)!=0) {
    errno=error;
    perror("\nLocking mutex: ");
    _exit(EXIT_FAILURE);
  }
  printf("Mutex locked\n");
  return;
}

void m_unlock(pthread_mutex_t* mtx) {
  int error;
  if(error=pthread_mutex_unlock(mtx)!=0) {
    errno=error;
    perror("\nUnlocking mutex: ");
    _exit(EXIT_FAILURE);
  }
  printf("Mutex unlocked\n");
  return;
}

void m_wait(pthread_cond_t* cond, pthread_mutex_t* mtx) {
  int error;
  if(error=pthread_cond_wait(cond, mtx)!=0) {
    errno=error;
    perror("\nPreparing to wait for cond: ");
    _exit(EXIT_FAILURE);
  }
  return;
}

void m_signal(pthread_cond_t* cond) {
  int error;
  if(error=pthread_cond_signal(cond)!=0) {
    errno=error;
    perror("\nSignaling cond: ");
    _exit(EXIT_FAILURE);
  }
  return;
}
