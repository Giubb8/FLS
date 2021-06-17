#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"chained_hash.h"
#include<string.h>
/*
typedef struct file_s{
    FILE* fp;
    int size;
    int order;
}file_s;*/


int main(int argc, char const *argv[])
{
    chained_hash_t* hasht=new_hash_table(10);
    printf("ciaon!\n");
    printf("numero di file da inserire:\n");
    int n;
    scanf("%d",&n);

    for(int i=0;i<n;i++) {
        char num[5];
        sprintf(num,"%d",i);
        printf("num è :%s\n",num);
        char name[10]="file";
        printf("name prima %s\n",name);
        strcat(name,num);
        printf("name: %s\n",name);
        FILE *fpp=fopen(name,"w+");
        file_s* newf=malloc(sizeof(file_s));
        newf->size=0;
        newf->order=i;
        newf->fp=fpp;
        strcpy(newf->fname,name);
        chained_hash_insert(hasht,i,*newf);
        
    }
    printhash(hasht);
    printf("QUA\n");
    file_s* filer=malloc(sizeof(file_s));
    printf("qui\n");
    chained_hash_search(hasht, 8, filer, "file8");
    printf("WEWE\n");
    char str[22]="ciao caro";
    printf("QUAQUA\n");
    printfiles(*filer);
    fwrite(str,1,22,filer->fp);
    printf("QUAQUdA\n");
    fread(stdout,2,22,filer->fp);

    chained_hash_delete(hasht,6,"file7");

    printf("CIAONE IN TEORIA HO QUASI FINITO\n");
    printhash(hasht);

    return 0;
}
