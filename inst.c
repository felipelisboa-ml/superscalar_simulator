#include "inst.h"
#include <stdlib.h>
#include <stdio.h>

inst_t * init_instruction(int size_rs, int init_lat, int id, char vec_sizes[MAXCHAR]){
  inst_t * t = (inst_t*) malloc(sizeof(inst_t));
  t->id = id;
  t->num_of_stations = size_rs;
  t->initial_exec_latency = init_lat;
  t->actual_exec_latency = init_lat;
  t->rs = malloc(size_rs*sizeof(reservation_station_t*));
  for(int j=0; j<size_rs; j++){
    int ctoi = (vec_sizes[j] - '0');
    //cópia
    t->rs[j] = &(*res_stations[ctoi]);
  }
  t->dep_down = NULL;
  t->dep_up = NULL;
  t->dep_to_solve = 0;
  t->done = 0;
  return t;
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
      down_inst->num_of_dep++;
      down_inst->dep_up[i]->inst_id = up_inst->id;
      down_inst->dep_up[i]->init_dep_latency = dep_val;
      down_inst->dep_up[i]->dep_latency = dep_val;
      break;
    }
    i++;
  }
}

static int count_up_deps(inst_t * inst){
  int count=0;
  if(inst->dep_up!=NULL)
    for(int i=0; i<MAXUPDEPS; i++)
      if(inst->dep_up[i]->inst_id != -1) count++;
  return count;
}

int calculate_up_deps(inst_t * inst){
  inst->dep_to_solve = count_up_deps(inst);
  return inst->dep_to_solve;
}

void manage_down_dependencies(inst_t * inst){
  int number_of_down_dep = get_size(inst->dep_down);
  for(int i=0; i<number_of_down_dep; i++){
    inst_t * inst_down = inst->dep_down->buffer[i];
    int number_of_up_dep = inst_down->num_of_dep;
    for(int j=0; j<number_of_up_dep; j++){
      pair_up_dependency * up_dep = inst_down->dep_up[j];
      if((up_dep->inst_id == inst->id)&&(up_dep->dep_latency > 0)){
	if(up_dep->dep_latency == up_dep->init_dep_latency)
	  up_dep->dep_latency = up_dep->dep_latency - (inst->initial_exec_latency - inst->actual_exec_latency);
	else
	  up_dep->dep_latency--;
	if(up_dep->dep_latency <=0)
	  inst_down->dep_to_solve--;
      }
    }
  }
}

void choose_rs(inst_t * inst){

  insert_element(inst->rs[0]->inst_buffer,inst);
  
  /* STILL TO IMPLEMENT CHOOSING THE LEAST BUSY
  int min = INT_MAX;
  int indice_res = 0;
  for(int j=0; j<get_numberofstations(inst); j++){
    
    if(get_size(inst->rs[j]->inst_buffer) < min){
      min = get_size(inst->rs[j]->inst_buffer);
      indice_res = j;
    }
  }rintf("Chosen RS for instruction %d: %d\n", new_inst->id, rs);
  insert_element(inst->rs[indice_res]->inst_buffer,inst);
  print_buffer(inst->rs[indice_res]->inst_buffer);
  return indice_res;
  */
}


int manage_own_latency(inst_t * inst){
  return ((inst->actual_exec_latency==0) ? 0 : --(inst->actual_exec_latency));
}

void show_host_rs(inst_t * inst){
  printf("Instruction %d can execute in stations: ", inst->id);
  for(int i=0; i<inst->num_of_stations; i++){
    printf("%d ", inst->rs[i]->id);
  }
  printf("\n");
}

void print_up_deps(inst_t * inst){
  for(int i=0; i<MAXUPDEPS; i++){
    if(inst->dep_up[i]->inst_id != -1)
      printf("Father: %d, latency: %d \n", inst->dep_up[i]->inst_id, inst->dep_up[i]->dep_latency);
  }
}

void print_deps(inst_t * inst){
  //Should print dependencies from dependant instructions
  for(int i=0; i<get_size(inst->dep_down); i++){
    inst_t * down = inst->dep_down->buffer[i];
    for(int j=0; j<(down->num_of_dep); j++){
      printf("Inst %d dependant of inst %d with latency : %d \n", down->id, down->dep_up[j]->inst_id, down->dep_up[j]->dep_latency);
    }
  }
}

int get_numberofstations(inst_t * inst){
  return inst->num_of_stations;
}

int get_init_num_of_dep(inst_t * inst){
  return inst->num_of_dep;
}




