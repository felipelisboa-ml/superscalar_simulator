#include "buffer.h"
#include <stdlib.h>

buffer_t * init_buffer(int size){
  buffer_t * b = (buffer_t *) malloc(sizeof(buffer_t));
  b->buffer = (inst_t**) malloc (size*sizeof(inst_t*));
  b->size = 0;
  return b;
}

void insert_element(buffer_t * buf, inst_t * added_inst){
  buf->buffer[buf->size] = added_inst;
  buf->size++;
}

int get_size(buffer_t * buf){
  return buf->size;
}
