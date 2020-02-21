#ifndef BUFFER_H
#define BUFFER_H

#include "inst.h"
#include "reservation_station.h"

typedef struct inst_t inst_t;
typedef struct buffer_t buffer_t;

struct buffer_t{
  inst_t ** buffer;
  int last,size;
};

//Inits the buffer
buffer_t * init_buffer(int size);
//Inserts new elements (FIFO policy)#include "buffer.h"

int insert_element(buffer_t * buf, inst_t * added_inst);
//Removes element (FIFO policy)
inst_t * remove_element(buffer_t * buf, int pos);
//Gets the current buffer size
int get_size(buffer_t * buf);
//Prints the buffer
void print_buffer(buffer_t * buf);
  
#endif
