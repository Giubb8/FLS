#include <stdlib.h>
#include<stdio.h>
#include <string.h>
#include "chained_hash.h"
#include<math.h>
/*
 * Implementazione base di una tabella hash (di interi) con liste di trabocco
 *
 * alpha = n/m (fattore di carico)
 * Rappresenta la lunghezza media delle liste di trabocco,
 * dove n è il numero di elementi presenti in tabella
 * ed m è il numero di celle della tabella.
 * È bene tenere alpha intorno al valore 0.5 (tabella piena per metà).
 */

 /* Cambiare questa define per scegliere la funzione hash da utilizzare
  * (vedere il file chained_hash.h per vedere le funzioni hash disponibili)
  */
//TODO cancellare gli usi ormai non più necessari della key
#define hash(k, m) hash_xor(k, m)
unsigned long compute_hash( char *str){//http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    int c;

    while ((c = (*str++)))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void printfiles(file_s file){
  printf("DENTRO PRINTFILES******************************************************\n");
  printf("name: %s size: %d,order : %d,puntatore %p\n",file.fname,file.size,file.order,file.fp);
  printf("***********************************************************************\n");
}

void print_listh(list_elem_th* list) {
    list_elem_th* curr = list;
    
    printf("Contenuto della lista:\n");
    while(curr != NULL) {
        printf("key %d ",curr->key);
        printfiles(curr->val);
        curr = curr->next;
    }
    printf("\n");
}

void printhash(chained_hash_t *T){
    for(int i=0;i<T->m;i++){
        printf("pos: %d ",i);
        print_listh(T->table[i]);
    }
}
/* Alloca una nuova tabella hash di m posizioni e ne restituisce il puntatore. */
chained_hash_t* new_hash_table(int m) {
    int i;
    
    /* Allocazione della memoria */
    chained_hash_t* T = malloc(sizeof(chained_hash_t));
    T->table = malloc(m * sizeof(list_elem_th*));
    T->m = m;
    
    for(i=0;i<m;i++) {
        /* Inizializzazione a NULL di tutta la tabella.
         * È utile per la fase di inserimento
         * (che controlla se una cella è NULL).
         */
        T->table[i] = NULL;
    }
    return T;
}

/* Deallocazione di una tabella hash di m posizioni. */
void hash_destroy(chained_hash_t* T) {
    int i;
    int m = T->m;
    for(i=0;i<m;i++) {
        list_elem_th* lista = T->table[i];
        
        /* Deallocazione dell'intera lista in posizione i */
        while(lista != NULL) {
            list_elem_th* tmp = lista;
            lista = lista->next; // Avanza nella lista
            free(tmp); // Libera il blocco precedente
        }
    }
    
    /* Deallocazione della tabella */
    free(T->table);
    /* Deallocazione della struct che contiene la tabella */
    free(T);
}

/* Inserimento di elem (con chiave k) nella tabella hash T.
 * NON non controlla se la chiave è già presente nella tabella.
 * Tempo: O(1 + alpha) = O(1)
 */
void chained_hash_insert(chained_hash_t* T, int k, file_s elem) {
    /* Calcolo dell'hash */
    //int posizione = hash(k, T->m);
    int posizione = ((compute_hash(elem.fname))%T->m);
    printf("posizione calcolata: %d\n",posizione);
    list_elem_th* new_elem = malloc(sizeof(list_elem_th));
    new_elem->key = k;
    new_elem->val = elem;
    new_elem->next = T->table[posizione]; // NULL se la lista era vuota
    
    T->table[posizione] = new_elem; // Inserimento in testa
}

/* Ricerca della chiave k nella tabella hash T.
 * Tramite *result si ottiene il valore associato alla chiave (NULL se assente)
 * Restituisce 0 se la chiave non è presente, 1 altrimenti.
 *
 * Tempo O(1 + alpha) = O(1)
 */
int chained_hash_search(chained_hash_t* T, int k, file_s* result,char* name) {
    int posizione = ((compute_hash(name))%T->m);
    list_elem_th* lista = T->table[posizione];
    while(lista != NULL && ((strcmp(lista->val.fname,name))!=0)){
        lista = lista->next;
    }
    if(lista == NULL) { // Non trovato
        result = NULL;
        return 0;
    }
    else {
        //printfiles(lista->val);
        *result = lista->val; 
        return 1; // Esito positivo
    }
}

/* Cancellazione dell'elemento di chiave k nella tabella hash T.
 * Tempo: O(1 + alpha) = O(1)
 */
void chained_hash_delete(chained_hash_t* T, int k,char * name) {
    //int pos = hash(k, T->m);
    int pos = ((compute_hash(name))%T->m);
    list_elem_th* current = T->table[pos];
    
    list_elem_th* previous = NULL;
    while(current != NULL && ((strcmp(current->val.fname,name))!=0)) {
        previous = current;
        current = current->next;
    }
    /* Current punterà al blocco della lista che contiene la chiave k, se c'è */
    
    if(current != NULL) { // Se k è effettivamente presente nella tabella
        if(previous == NULL) { // Cancellazione in testa alla lista
            /* Avanziamo il puntatore alla testa della lista
             * e deallochiamo la "vecchia" testa (puntata da current)
             */
            T->table[pos] = current->next;
            remove(name);
            free(current);
        }
        else { // Cancellazione in mezzo alla lista
            previous->next = current->next; // "Salta" il blocco da cancellare
            remove(name);
            free(current);
        }
    }
}
