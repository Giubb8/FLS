
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

typedef struct message{
  char op;
  char args[439];
}msg;

void main(int argc, char const *argv[]){
  msg m;
  m.op ='z';
  char buff[256];
  printf("PRE SCRITTURA :  %c %s\n",m.op,m.args);
    
  //while(fscanf(stdin,"-%c%[^\n]%[^\n]",&m.op,NULL,m.args)){
   while(fgets(buff,255,stdin)){
       printf("letto %s",buff);
       char * ptr;
       char * ptr2;
       ptr=strchr(buff,'-');
       if(ptr==NULL){
        perror("Formato Input sbagliato");
        exit(EXIT_FAILURE);
       }
       ptr2=strchr(buff,' ');
       *ptr++;
       char dodo=*ptr;
       
       printf("dodo %c\n",dodo);
       printf("wewe %s\n",ptr2);
       //fgets(buff)
   } 
    //Legge da stdinput
    
    //printf("COSA HO RICEVUTO :  %c %s",m.op,m.args);
    //impacchetta
    //spedisce
  //}
}
