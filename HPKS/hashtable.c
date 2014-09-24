#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

typedef struct _node{
  char *name;
  char *desc;
  struct _node *next;
}node;

#define HASHSIZE 101
static node* hashtab[HASHSIZE];

void init_hashtab(){
  int i;
  for(i=0;i<HASHSIZE;i++)
    hashtab[i]=NULL;
}

unsigned int hash(char *s){
  unsigned int h=0;
  for(;*s;s++)
    h=*s+h*31;
  return h%HASHSIZE;
}

node* lookup(char *n){
  unsigned int hi=hash(n);
  node* np=hashtab[hi];
  for(;np!=NULL;np=np->next){
    if(!strcmp(np->name,n))
      return np;
  }

  return NULL;
}

char* m_strdup(char *o){
  int l=strlen(o)+1;
  char *ns=(char*)malloc(l*sizeof(char));
  strcpy(ns,o);
  if(ns==NULL)
    return NULL;
  else
    return ns;
}

char* get_hashtab(char* name){
  node* n=lookup(name);
  if(n==NULL)
    return NULL;
  else
    return n->desc;
}

int add_hashtab(char* name,char* desc){
  unsigned int hi;
  node* np;
  if((np=lookup(name))==NULL){
    hi=hash(name);
    np=(node*)malloc(sizeof(node));
    if(np==NULL)
      return 0;
    np->name=m_strdup(name);
    if(np->name==NULL) return 0;
    np->next=hashtab[hi];
    hashtab[hi]=np;
  }
  else
    free(np->desc);
  np->desc=m_strdup(desc);
  if(np->desc==NULL) return 0;

  return 1;
}

/* A pretty useless but good debugging function,
   which simply displays the hashtable in (key.value) pairs
*/
void displaytable(){
  int i;
  node *t;
  for(i=0;i<HASHSIZE;i++){
    if(hashtab[i]==NULL)
      printf("()\n");
    else{
      t=hashtab[i];
      printf("(");
      for(;t!=NULL;t=t->next)
	printf("(%s.%s) ",t->name,t->desc);
      printf(")\n");
    }
  }
}

void cleanup_hashtab(){
  int i;
  node *np,*t;
  for(i=0;i<HASHSIZE;i++){
    if(hashtab[i]!=NULL){
      np=hashtab[i];
      while(np!=NULL){
	t=np->next;
	free(np->name);
	free(np->desc);
	free(np);
	np=t;
      }
    }
  }
}
