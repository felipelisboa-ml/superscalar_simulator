#include "reservation_station.h"
#include <stdlib.h>

reservation_station_t * init_res_station(int size_buffer){
  reservation_station_t * r = (reservation_station_t *) malloc(sizeof(reservation_station_t));
  r->inst_id = NULL;
  r->inst_buffer = init_buffer(size_buffer);
  //r->inst_buffer->init_buffer(size_buffer);
  return r;
}

void * is_occupied(reservation_station_t * rs){
  if(rs->inst_id == NULL) return NULL;
}
