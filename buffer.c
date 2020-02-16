#include "buffer.h"
#include <stdlib.h>

buffer_t * init_buffer(int size){
  buffer_t * b = (buffer_t *) malloc(sizeof(buffer_t));
  b->buffer = (inst_t**) malloc (size*sizeof(inst_t*));
  b->last = 0;
  b->size = size;
  return b;
}
 
//Add following FIFO policy (adds into the end of the list)
int insert_element(buffer_t * buf, inst_t * added_inst){
  if(buf->last >= buf->size)
    return 0;
  else{
    buf->last++;
    buf->buffer[buf->last] = added_inst;
    return 1;
  }
}

//Remove from a given position
inst_t * remove_element(buffer_t * buf, int pos){
  if(buf->buffer[pos] == NULL)
    return NULL;
  else {
    inst_t * ref = buf->buffer[pos];
    for(int i=pos; i<buf->last; i++) buf->buffer[i] = buf->buffer[i+1];
    buf->buffer[buf->last]=NULL;
    return ref;
  }
}

int get_size(buffer_t * buf){
  return buf->size;
}
