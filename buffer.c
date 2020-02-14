#include "buffer.h"
#include <stdlib.h>

buffer_t * init_buffer(int size){
  buffer_t * b = (buffer_t *) malloc(sizeof(buffer_t));
  b->buffer = (inst_t**) malloc (size*sizeof(inst_t*));
  b->size = 0;
  return b;
}

//Add following FIFO policy
void insert_element(buffer_t * buf, inst_t * added_inst){
  buf->buffer[buf->size] = added_inst;
  buf->size++;
}

//Remove following FIFO policy
inst_t * remove_element(buffer_t * buf){
  inst_t * ref = buf->buffer[buf->size];
  buf->buffer[buf->size]=NULL;
  free(buf->buffer[buf->size]);
  return ref;
}

int get_size(buffer_t * buf){
  return buf->size;
}
