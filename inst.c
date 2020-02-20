#include "inst.h"
#include <stdlib.h>
#include <stdio.h>

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
  t->dep_up = NULL;
  return t;
}

void show_host_rs(inst_t * inst){
  printf("Instruction %d can execute in stations: ", inst->id);
  for(int i=0; i<inst->num_of_stations; i++){
    printf("%d ", inst->rs[i]->id);
  }
  printf("\n");
}

static inst_t * allocate_updependency_buffer(inst_t * inst){
  inst->dep_up = (pair_up_dependency **) malloc(MAXUPDEPS*sizeof(pair_up_dependency*));
  for(int i=0; i<MAXUPDEPS; i++){
    inst->dep_up[i] = (pair_up_dependency*) malloc(sizeof(pair_up_dependency));
    inst->dep_up[i]->inst_id = -1;
  }
  return inst;
}

void config_dependencies(inst_t * up_inst, inst_t * down_inst, int dep_val){
  //Down instruction is only added to the father's dep_down
  if(up_inst->dep_down == NULL)
    up_inst->dep_down = init_buffer(MAXDOWNDEPS);
  insert_element(up_inst->dep_down,down_inst);
  //Child must have its dep_up alocated if it hasn't been yet
  if(down_inst->dep_up == NULL)
    down_inst = allocate_updependency_buffer(down_inst);
  int i=0;
  while(1){
    //Adds to the first free up dependency spot found
    if(down_inst->dep_up[i]->inst_id == -1){
      down_inst->dep_up[i]->inst_id = up_inst->id;
      down_inst->dep_up[i]->init_dep_latency = dep_val;
      down_inst->dep_up[i]->dep_latency = dep_val;
      break;
    }
    i++;
  }
}

int get_numberofstations(inst_t * inst){
  return inst->num_of_stations;
}

int get_init_num_of_dep(inst_t * inst){
  return inst->num_of_dep;
}
