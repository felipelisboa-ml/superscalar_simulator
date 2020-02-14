#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "inst.h"
#include "buffer.h"

typedef struct inst_t inst_t;
typedef struct buffer_t buffer_t;
typedef struct reservation_station_t reservation_station_t;

reservation_station_t ** res_stations;

struct reservation_station_t{
  buffer_t * inst_buffer;
  inst_t * inst_id;
};

//Initiates structures
reservation_station_t * init_res_station(int size_buffer);

//See's if buffer is occupied
void * is_occupied(reservation_station_t * rs);

#endif
