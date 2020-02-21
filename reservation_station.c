#include "reservation_station.h"
#include <stdlib.h>

reservation_station_t * init_res_station(int id, int size_buffer){
  reservation_station_t * r = (reservation_station_t *) malloc(sizeof(reservation_station_t));
  r->id = id;
  r->inst_id = NULL;
  r->inst_buffer = init_buffer(size_buffer);
  //r->inst_buffer->init_buffer(size_buffer);
  return r;
}

void * is_occupied(reservation_station_t * rs){
  if(rs->inst_id == NULL) return NULL;
}

int check_if_empty(reservation_station_t * rs){
  //Return 0 if there is at least one instruction that isn't NULL
  for(int i=0; i<rs->inst_buffer->size; i++){
    if(rs->inst_buffer->buffer[i]!=NULL)
      return 0;
  }
  //Return 1 if it's empty
  return 1;
}
