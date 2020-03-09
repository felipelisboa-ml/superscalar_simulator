#ifndef CIRC_BUFFER_H
#define CIRC_BUFFER_H

#include "inst.h"

typedef struct {
  int first, last, size, max_size;
  inst_t ** buffer;
} circ_buffer_t;

// Allocate and initialize the circular buffer structure
circ_buffer_t * circ_buffer_init(int size);

// Remove an element from circular buffer. When empty, return NULL.
void * circ_buffer_get(circ_buffer_t * b);

// Read (and do not remove) an element from circular buffer. When
// empty, return NULL.
void * circ_buffer_read(circ_buffer_t * b);

// Append an element into circular buffer. When full, return 0.
int circ_buffer_put(circ_buffer_t * b, void * d);

int circ_buffer_size(circ_buffer_t * b);

void free_circ_buffer(circ_buffer_t * b);

#endif
