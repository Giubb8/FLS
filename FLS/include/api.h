
int fd;
/*
Viene aperta una connessione AF_UNIX al socket file sockname. Se il server non accetta immediatamente la
richiesta di connessione, la connessione da parte del client viene ripetuta dopo ‘msec’ millisecondi e fino allo
scadere del tempo assoluto ‘abstime’ specificato come terzo argomento. Ritorna 0 in caso di successo, -1 in caso
di fallimento, errno viene settato opportunamente.
*/
int openConnection(const char* sockname, double msec, const struct timespec abstime){
    double waitime=msec/1000.0;
    double waited=0;
    fd=m_socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un sa;
    strcpy(sa.sun_path,sockname);
    sa.sun_family=AF_UNIX;
    while( ((connect(fd,(struct sockaddr*)&sa,sizeof(sa))) ==-1) && (abstime.tv_sec>waited) ){
      if(errno==ENOENT){
        sleep(waitime);
        waited+=(waitime);
        if(DEBUG){printf("waited e' %lf\n",waited);}
        if(DEBUG){printf("client sta aspettando da: %lf ...\n",waited);}
      }
      else return(-1);
    }
    if(waited<abstime.tv_sec){
      if(DEBUG){printf("client connesso\n");}
      return 0;
    }
    else return -1;
  }

/*
Chiude la connessione AF_UNIX associata al socket file sockname. Ritorna 0 in caso di successo, -1 in caso di
fallimento, errno viene settato opportunamente.
*/
int closeConnection(const char* sockname);

/*Richiesta di apertura o di creazione di un file. La semantica della openFile dipende dai flags passati come secondo
argomento che possono essere O_CREATE ed O_LOCK. Se viene passato il flag O_CREATE ed il file esiste già
memorizzato nel server, oppure il file non esiste ed il flag O_CREATE non è stato specificato, viene ritornato un
errore. In caso di successo, il file viene sempre aperto in lettura e scrittura, ed in particolare le scritture possono
avvenire solo in append. Se viene passato il flag O_LOCK (eventualmente in OR con O_CREATE) il file viene
aperto e/o creato in modalità locked, che vuol dire che l’unico che può leggere o scrivere il file ‘pathname’ è il
processo che lo ha aperto. Il flag O_LOCK può essere esplicitamente resettato utilizzando la chiamata unlockFile,
descritta di seguito.
Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente*/
int openFile(const char* pathname, int flags){
  if(DEBUG){printf("dentro OPEN FILE, ricevuto %s\n",pathname);}
  msg message;
  int err;
  message.op='o';
  message.more=1;
  strcpy(message.args,pathname);
  message.flag=flags; //char buffer[MAXSTRLEN];
  int w=writen(fd,&message,sizeof(message));//TODO GESTIONE ERRORE DA PARTE DEL SERVER
  if(DEBUG){printf("SCRITTI SUL SERVER w=%d\n",w);}
  int r=readn(fd,&err,sizeof(err));  
  if(DEBUG){printf("RICEVUTO DAL SERVER %d,LETTI %d\n",err,r);}
  return err;
}

/*Legge tutto il contenuto del file dal server (se esiste) ritornando un puntatore ad un'area allocata sullo heap nel
parametro ‘buf’, mentre ‘size’ conterrà la dimensione del buffer dati (ossia la dimensione in bytes del file letto). In
caso di errore, ‘buf‘e ‘size’ non sono validi. Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene
settato opportunamente.
*/
int readFile(const char* pathname, void** buf, size_t* size){
  //preparo il messaggio da inviare al file
  printf("dentro readfile\n");
  rep reply;
  msg message;
  message.op='r';
  strcpy(message.args,pathname);
  printf("messaggio che sto inviando al server %c %s\n",message.op,message.args);
  //invio il messaggio per la lettura del file al server e aspetto la risposta
  int w=writen(fd,&message,sizeof(message));//TODO GESTIONE ERRORE DA PARTE DEL SERVER
  printf("dopo writen\n");
  int r=readn(fd,&reply,sizeof(reply));//ho comunque il buffer puntato quindi posso copiare nella cartella nel chiamante 
  printf("r: %d args %s\n",r,reply.args);
  //copio nel buf la risposta del server
  memcpy(buf,reply.args,(strlen(reply.args)+1));
  printf("letto dal server in readfile : %s\n",(char*)buf);
  if(strlen((char*)buf)>=0){//TODO controllare se va bene ma penso di si
    return 0;
  }
  else return -1;
}
/*Richiede al server la lettura di ‘N’ files qualsiasi da memorizzare nella directory ‘dirname’ lato client. Se il server
ha meno di ‘N’ file disponibili, li invia tutti. Se N<=0 la richiesta al server è quella di leggere tutti i file
memorizzati al suo interno. Ritorna un valore maggiore o uguale a 0 in caso di successo (cioè ritorna il n. di file
effettivamente letti), -1 in caso di fallimento, errno viene settato opportunamente.
*/
int readNFiles(int N, const char* dirname);
/*Scrive tutto il file puntato da pathname nel file server. Ritorna successo solo se la precedente operazione,
terminata con successo, è stata openFile(pathname, O_CREATE| O_LOCK). Se ‘dirname’ è diverso da NULL, il
file eventualmente spedito dal server perchè espulso dalla cache per far posto al file ‘pathname’ dovrà essere
scritto in ‘dirname’; Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
*/
int writeFile(const char* pathname, const char* dirname);
/*Richiesta di scrivere in append al file ‘pathname‘ i ‘size‘ bytes contenuti nel buffer ‘buf’. L’operazione di append
nel file è garantita essere atomica dal file server. Se ‘dirname’ è diverso da NULL, il file eventualmente spedito
dal server perchè espulso dalla cache per far posto ai nuovi dati di ‘pathname’ dovrà essere scritto in ‘dirname’;
Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
*/
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname);
/*In caso di successo setta il flag O_LOCK al file. Se il file era stato aperto/creato con il flag O_LOCK e la
richiesta proviene dallo stesso processo, oppure se il file non ha il flag O_LOCK settato, l’operazione termina
immediatamente con successo, altrimenti l’operazione non viene completata fino a quando il flag O_LOCK non
viene resettato dal detentore della lock. L’ordine di acquisizione della lock sul file non è specificato. Ritorna 0 in
caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
*/

/* Richiesta di chiusura del file puntato da ‘pathname’.Eventuali operazioni sul file dopo la closeFile falliscono.
Ritorna 0 in caso di successo,-1 in caso di fallimento,errno viene settato opportunamente*/
int closeFile(const char* pathname);


/*Rimuove il file cancellandolo dal file storage server. L’operazione fallisce se il file non è in stato locked, o è in
stato locked da parte di un processo client diverso da chi effettua la removeFile.
*/
int removeFile(const char* pathname);
