#ifndef CIRC_BUFFER_H
#define CIRC_BUFFER_H

#include "inst.h"

typedef struct circ_buffer_t circ_buffer_t;

struct circ_buffer_t{
    inst_t ** circ_buffer;
    int head, tail;
    int size;
};

//Inits the buffer
circ_buffer_t * init_buffer_circ(int size);

//Inserts new elements at the tail if possible
int insert_element_circ(circ_buffer_t * buf, inst_t * added_inst);

//Removes element from the head
inst_t * remove_element_circ(circ_buffer_t * buf);

//Prints the buffer
void print_buffer_circ(circ_buffer_t * buf);

#endif
