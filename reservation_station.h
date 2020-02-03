#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "inst.h"
#include "buffer.h"

typedef struct{
  buffer_t * inst_buffer;
  inst_t * inst_id;
  int occupied;
} reservation_station_t;

//Alocate memory for buffers;
reservation_station_t * init_res_station();

//See's if buffer is occupied
int is_occupied(reservation_station_t * rs);

#endif
