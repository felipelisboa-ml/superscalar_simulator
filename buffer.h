#ifndef BUFFER_H
#define BUFFER_H

#include "inst.h"

typedef struct{
  inst_t ** buffer;
  int size;
} buffer_t;

buffer_t * init_buffer(int size);

//Inserts new elements (FIFO policy)
void insert(inst_t * added_inst);

//Gets the current buffer size
int get_size();

//Returns elements (Without removing from buffer)
inst_t * get();
  
//Removes element (FIFO policy)
inst_t * remove();

#endif
