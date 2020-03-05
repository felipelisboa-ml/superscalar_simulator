#include "circ_buffer.h"
#include <stdlib.h>
#include <stdio.h>


//Inits the buffer
circ_buffer_t * init_buffer_circ(int size){
  circ_buffer_t * b = (circ_buffer_t *) malloc(sizeof(circ_buffer_t));
  b->circ_buffer = (inst_t**) malloc (size*sizeof(inst_t*));
  b->head = 0;
  b->tail = -1;
  b->size = size;
  return b;
}

//Inserts new elements at the tail if it's possible
int insert_element_circ(circ_buffer_t * buf, inst_t * added_inst){
  int next=(buf->tail+1) % (buf->size);
  if(next == buf->head && buf->tail != -1)   // circular buffer is full
    return 0;
  else{
    buf->circ_buffer[next] = added_inst;
    buf->tail = next;
    return 1;
  }
}

//Removes element from the head
inst_t * remove_element_circ(circ_buffer_t * buf){
  int next;

  if (buf->circ_buffer[buf->head] == NULL)      // if the head points to NULL value => we don't have any data
    return NULL;
  else{
    next = (buf->head + 1) % (buf->size);    // next is where head will point to after this remove.
    inst_t * ref = buf->circ_buffer[buf->head];
    //buf->circ_buffer[buf->head] = NULL;
    buf->head = next;                       // head to next offset
    return ref;
 }
}

//Prints the circular buffer
void print_buffer_circ(circ_buffer_t * buf){
  int i = buf->head;
  if(buf->circ_buffer[i] != NULL){         //if not empty
    // if head < tail
    while(i <= buf->tail){
      printf("Instruction %d with done = %d\n",buf->circ_buffer[i]->id,buf->circ_buffer[i]->done);
      i = (i+1)%(buf->size);
    }
    // to be continued for the case head > tail
  }
}

int get_size_circ(circ_buffer_t * buf){
	inst_t * tmp = buf->circ_buffer[buf->head];
	int size = 0,index=0;	
	while(tmp != NULL){
		size++;
		index++;
		tmp = buf->circ_buffer[index];
	}	
	return size;
}
