#include "buffer.h"

buffer_t * init_buffer(int size){
  buffer_t * b = (buffer_t *) malloc(sizeof(buffer_t));
  b->size = 0;
  b->buffer = (inst*) malloc (size*sizeof(inst_t*));
  return b;
}
