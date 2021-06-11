
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


int main(int argc, char const *argv[])
{   FILE* fp=NULL;
    fp=fopen("provafile","w+");
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    printf("size da vuoto:%ld\n",sz);
    char* text="ciao,questo è un testo\n";
    int szt=strlen(text);
    printf("size text %d\n",szt);
    fwrite(text, szt, 1, fp);
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    printf("size da pieno:%ld\n",sz);

    return 0;
}
