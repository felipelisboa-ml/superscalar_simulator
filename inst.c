#include "inst.h"
#include <stdlib.h>

/* Most of the initialization work is done while reading the file,
   so this function isn't called at any point
*/

inst_t * init_instruction(int size_rs, int init_lat, int id, char vec_sizes[MAXCHAR]){
  inst_t * t = (inst_t*) malloc(sizeof(inst_t));
  t->id = id;
  t->num_of_stations = size_rs;
  t->initial_exec_latency = init_lat;
  t->actual_exec_latency = init_lat;
  t->rs = malloc(size_rs*sizeof(reservation_station_t*));
  for(int j=0; j<size_rs; j++){
    int ctoi = (vec_sizes[j] - '0'); 
    t->rs[j] = res_stations[ctoi];
  }
  return t;
}

void show_host_rs(inst_t * inst){
  printf("Instruction %d can execute in stations: ", inst->id);
  for(int i=0; i<inst->num_of_stations; i++){
    printf("%d ", inst->rs[i]->id);
  }
  printf("\n");
}

int get_numberofstations(inst_t * inst){
  return inst->num_of_stations;
}

int get_init_num_of_dep(inst_t * inst){
  return inst->num_of_dep;
}
