
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
