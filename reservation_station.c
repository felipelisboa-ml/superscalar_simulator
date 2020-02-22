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
  return rs->inst_id;
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

int put_into_FU(reservation_station_t * res){
  int del_index=0;
  int limite = get_size(res->inst_buffer);
  if(limite == 0)
    return -3;
  else{
    for(int j=0; j < limite; j++){
      if(is_occupied(res)!=NULL)
	return -2;
      else if(res->inst_buffer->buffer[j]->dep_to_solve > 0)
	return -1;
      else{
	res->inst_id = res->inst_buffer->buffer[j];
	del_index = j;
	break;
      }
    }
  }
  return del_index;
}
