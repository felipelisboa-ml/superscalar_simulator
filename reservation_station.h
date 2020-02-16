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
  int id;
};

/* This function is used to initialize the structure with an id and also intiialize its buffer
 */
reservation_station_t * init_res_station(int id, int size_buffer);

/* Checks inst_id pointer to see if the station is currently occupied
 */
void * is_occupied(reservation_station_t * rs);

#endif
