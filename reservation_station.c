#include "reservation_station.h"
reservation_station_t * init_res_station(int size_buffer){
  reservation_station_t * r = (reservation_station_t *) malloc(sizeof(reservation_station_t));
  r->occupied = 0;
  r->inst_id = NULL;
  r->inst_buffer->init_buffer(size_buffer);
  return r;
}
