#include "circ_buffer.h"
#include <stdlib.h>
#include <stdio.h>

circ_buffer_t * circ_buffer_init(int max_size) {
  circ_buffer_t * b =
    (circ_buffer_t *)malloc(sizeof(circ_buffer_t));
  b->first = 0;
  b->last  = -1;
  b->size = 0;
  b->max_size = max_size;
  b->buffer = (inst_t **)malloc(max_size*sizeof(inst_t*));
  return b;
}

void * circ_buffer_get(circ_buffer_t * b){
  void * d;
  if (b->size == 0) return NULL;
  d = b->buffer[b->first];
  b->first = (b-> first + 1) % b->max_size;
  b->size--;
  return d;
}

void * circ_buffer_read(circ_buffer_t * b) {
  if (b->size == 0) return NULL;
  return b->buffer[b->first];
}


int circ_buffer_put(circ_buffer_t * b, void * d){
  if (b->size == b->max_size) return 0;
  b->last = (b->last + 1) % b->max_size;
  b->buffer[b->last] = d;
  b->size++;
  return 1;
}

int circ_buffer_size(circ_buffer_t * b) {
  return b->size;
}
