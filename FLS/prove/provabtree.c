
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "btreep.h"

/* ---------- Private function declarations ---------- */
static btnode node_Minimum(btnode);
static btnode node_Maximum(btnode);
static btnode node_Search(BTREE, btnode, void *);
static void node_Close(btnode);
static btnode node_Successor(btnode);
static btnode node_Make(BTREE tree, void *data);
static void btree_reallyPrint(BTREE, btnode, FILE *, unsigned int);
 
/* ---------------------------------------------------------------------- */
BTREE btree_Create(size_t size, int (*cmp)(const void *, const void *))
{
  BTREE ret;

  ret = malloc(sizeof(struct _btree_struct));
  if (ret) {
    ret->root = NULL;
    ret->node_size = sizeof(struct node);
    ret->elem_size = size;
    ret->cmp = cmp;
  }
  return ret;
}

 
/* ---------------------------------------------------------------------- */
int btree_Search(BTREE tree, void *key, void *ret)
{
  btnode node;

  node = root(tree);

  node = node_Search(tree, node, key);

  if (node) {
       if (ret) data_copy(tree, ret, data(tree, node));
    return 0;
  }
  else {
    return 1;
  }
}

/* ---------------------------------------------------------------------- */
static btnode node_Search(BTREE tree, btnode node, void *key)
{
  int result;

  if (node) {
    while (node && (result = data_compare(tree, data(tree, node), key))) {
      if (result > 0) {
	node = left(node);
      }
      else {
	node = right(node);
      }
    }
  }
  return node;
}

 
/* ---------------------------------------------------------------------- */
int btree_Minimum(BTREE tree, void *ret)
{
  btnode node;

  node = node_Minimum(root(tree));
  if (node) {
    data_copy(tree, ret, data(tree, node));
    return 0;
  }
  return 1;
}

/* ---------------------------------------------------------------------- */
static btnode node_Minimum(btnode node)
{
  if (node) {
    while (left(node)) {
      node = left(node);
    }
  }
  return node;
}

 
/* ---------------------------------------------------------------------- */
int btree_Maximum(BTREE tree, void *ret)
{
  btnode node;

  node = node_Maximum(root(tree));
  if (node) {
    data_copy(tree, ret, data(tree, node));
    return 0;
  }
  return 1;
}

/* ---------------------------------------------------------------------- */
static btnode node_Maximum(btnode node)
{
  if (node) {
    while (right(node)) {
      node = right(node);
    }
  }
  return node;
}

 
/* ---------------------------------------------------------------------- */
int btree_Empty(BTREE tree)
{
  return root(tree) ? 0 : 1;
}
 

/* ---------------------------------------------------------------------- */
int btree_Successor(BTREE tree, void *key, void *ret)
{
  btnode node;

  if (!root(tree)) return 1;

  node = node_Search(tree, root(tree), key);
  
  if (!node) return 1;

  node = node_Successor(node);

  if (node) {
    data_copy(tree, ret, data(tree, node));
    return 0;
  }
  return 1;
}

/* ---------------------------------------------------------------------- */
static btnode node_Successor(btnode node)
{
  btnode node2;

  if (right(node)) {
    node = node_Minimum(right(node));
  }
  else {
    node2 = parent(node);
    while (node2 && node == right(node2)) {
      node = node2;
      node2 = parent(node);
    }
    node = node2;
  }
  return node;
}
 
/* ---------------------------------------------------------------------- */
int btree_Predecessor(BTREE tree, void *key, void *ret)
{
  btnode node, node2;

  if (!root(tree)) return 1;

  node = node_Search(tree, root(tree), key);
  
  if (!node) return 1;

  if (left(node)) {
    node = node_Maximum(left(node));
  }
  else {
    node2 = parent(node);
    while (node2 && node == left(node2)) {
      node = node2;
      node2 = parent(node);
    }
    node = node2;
  }
  
  if (node) {
    data_copy(tree, ret, data(tree, node));
    return 0;
  }
  return 1;
}
 
/* ---------------------------------------------------------------------- */
int btree_Insert(BTREE tree, void *data)
{
  btnode parent, node, newnode;

  newnode = node_Make(tree, data);
  if (!newnode) {
    return 1;
  }

  parent = NULL;
  node = root(tree);

  while (node) {
    parent = node;
    if (data_compare(tree, data, data(tree, node)) < 0) {
      node = left(node);
    }
    else {
      node = right(node);
    }
  }

  parent(newnode) = parent;

  if (!parent) {
    root(tree) = newnode;
  }
  else {
    if (data_compare(tree, data, data(tree, parent)) < 0) {
      left(parent) = newnode;
    }
    else {
      right(parent) = newnode;
    }
  }
  return 0;
}

 
/* ---------------------------------------------------------------------- */
int btree_Delete(BTREE tree, void *data)
{
  btnode node;
  btnode remove;
  btnode other;

  if (!root(tree)) return 1;

  node = node_Search(tree, root(tree), data);

  if (!node) return 1;
  
  if (!left(node) || !right(node)) {
    remove = node;
  }
  else {
    remove = node_Successor(node);
  }

  if (left(remove)) {
    other = left(remove);
  }
  else {
    other = right(remove);
  }

  if (other) {
    parent(other) = parent(remove);
  }
  
  if (!parent(remove)) {
    root(tree) = other;
  }
  else {
    if (remove == left(parent(remove))) {
      left(parent(remove)) = other;
    }
    else {
      right(parent(remove)) = other;
    }
  }

  if (node != remove) {
    data_copy(tree, data(tree, node), data(tree, remove));
  }
  free(node);
  return 0;
}

 
/* ---------------------------------------------------------------------- */
void btree_Destroy(BTREE tree)
{
  if (root(tree)) {
    node_Close(root(tree));
  }
  free(tree);
}

/* ---------------------------------------------------------------------- */
static void node_Close(btnode node)
{
  if (left(node)) {
    node_Close(left(node));
  }
  if (right(node)) {
    node_Close(right(node));
  }
  free(node);
}

 
/* ---------------------------------------------------------------------- */
static btnode node_Make(BTREE tree, void *data)
{
  btnode ret;

  ret = malloc(node_size(tree) + elem_size(tree));

  if (ret) {
    data_copy(tree, data(tree, ret), data);
    parent(ret) = left(ret) = right(ret) = NULL;
  }
  return ret;
}

/* ------------------------------------------------------------------------ */
/* 
  Note that this routine assumes that the data elements are char *'s
 */
void btree_print(BTREE tree, void *where)
{
  btree_reallyPrint(tree, root(tree), (FILE *)where, 2);
}

/* ------------------------------------------------------------------------ */
void btree_reallyPrint(BTREE tree, btnode node, FILE *where, unsigned int spaces)
{
  fprintf(where, "%*s%s\n", spaces, " ", 
	  (node ?
	   *(char **)data(tree, node) :
	   "NULL"));
  if (node) {
    btree_reallyPrint(tree, left(node), where, spaces + 4);
    btree_reallyPrint(tree, right(node), where, spaces + 4);
  }
}

void printfiles(files file){
  printf("dentro printfiles\n");
  printf("size: %d,order : %d,puntatore %p\n",file.size,file.order,file.fp);
}
void btreep(BTREE tree,){
  if(tree->root==NULL){
    return;
  }
  else{
    btreep();
  }
}
typedef struct files{
    FILE* fp;
    int size;
    int order;
}files;

int cmp(const void* a, const void* b){
    const int *pa=(const int*)a;
    const int *pb=(const int*)b;

    return (*pa>*pb)-(*pa<*pb);
}
void printfiles(files file){
  printf("dentro printfiles\n");
  printf("size: %d,order : %d,puntatore %p\n",file.size,file.order,file.fp);
}
int main(int argc, char const *argv[])
{
    BTREE btree=btree_Create(sizeof(files),&cmp);
    int n;
    scanf("%d",&n);
    for(int i=0;i<n;i++) {
        printf("inizio for\n");
        char num[5];
        sprintf(num,"%d",i);
        printf("num è :%s\n",num);
        char name[10]="file";
        printf("name prima %s\n",name);
        strcat(name,num);
        printf("name: %s\n",name);
        FILE *fpp=fopen(name,"w+");
        printf("PUNTATORE È : %p\n",fpp);
        files* newf=malloc(sizeof(files));
        newf->size=0;
        newf->order=i;
        newf->fp=fpp;
        printfiles(*newf);
        btree_Insert(btree,newf);
        printf("ciaone\n");
        
    }
    files* toget=malloc(sizeof(files));
    btree_Maximum(btree,toget);
    printf("IL MASSIMO:\n");
    printfiles(*toget);

    printf("IL MINIMO:\n");
    files* toget2=malloc(sizeof(files));
    btree_Maximum(btree,toget2);
    printfiles(*toget2);

    btree_reallyPrint();
    return 0;
}
