#include <stdlib.h>
#include <stdio.h>
#include "mylist.h"

void mylist_init(struct mylist* l)
{
  // TODO: Fill it
  l->head =NULL;
}

void mylist_destroy(struct mylist* l)
{
  // TODO: Fill it
  struct mylist_node* cur = l->head;
  
  while(cur !=NULL){
    l->head = cur->next;
    free(cur);
    cur = l->head;
  }
  free(cur);
  free(l->head);

}

void mylist_insert(
    struct mylist* l,
    struct mylist_node* before, int data)
{
  // TODO: Fill it
  struct mylist_node* newnode = (struct mylist_node*)malloc(sizeof(struct mylist_node));
  newnode->data = data;
  if(before == NULL){
    if(l->head ==NULL){
      l->head = newnode;
      newnode->next = NULL;
    }
    else{
      newnode->next =l->head;
      l->head = newnode;
    }
  }
  else{
    newnode->next = before->next;
    before->next = newnode;
  }
  newnode = NULL;
  free(newnode);
}

void mylist_remove(
    struct mylist* l,
    struct mylist_node* target)
{
  // TODO: Fill it
  struct mylist_node* cur =l->head;
  if(cur ==NULL){ // no head
    free(cur);
  }
  else if(cur== target){ //equals target head
    l->head = cur->next; //change head 
    free(cur); 
  }
  else{ // different target head
    while(cur->next != target){ 
      cur = cur->next; // now cur is target before
    }
    cur->next = target->next;
    free(target);
  }
}

struct mylist_node* mylist_find(struct mylist* l, int target)
{
  // TODO: Fill it
  struct mylist_node* cur = l->head;
  while(cur!=NULL){
    if(cur->data == target){
      return cur;
    }
    else{
      if(cur->next==NULL) 
        return NULL;
      else{
        cur = cur->next;
      }
    }
  } 
  cur = NULL;
  free(cur);
  return NULL;// If not found
}

struct mylist_node* mylist_get_head(struct mylist* l)
{
  // TODO: Fill it
  if(l->head ==NULL) 
    return NULL;
  else
    return l->head;
}

void mylist_print(const struct mylist* l)
{

  for (struct mylist_node* pointer = l->head;
      pointer != NULL;
      pointer = pointer->next) {
    printf("%d\n", pointer->data);
      }

}

