#ifndef BUFFER_H
#define BUFFER_H

#include "inst.h"
#include "reservation_station.h"

typedef struct inst_t inst_t;
typedef struct buffer_t buffer_t;

struct buffer_t{
  inst_t ** buffer;
  int size;
};

//Inits the buffer
buffer_t * init_buffer(int size);
//Inserts new elements (FIFO policy)
void insert_element(buffer_t * buf, inst_t * added_inst);
//Removes element (FIFO policy)
void remove_element(int pos);
//Gets the current buffer size
int get_size();
//Returns elements (Without removing from buffer)
inst_t * get();
  
#endif
