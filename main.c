#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "inst.h"
#include "reservation_station.h"

//This parameters will be defined by the input file
#define SIZE_MAX_INSTRUCTION_BUFFER 16

reservation_station_t ** res_stations;
buffer_t * inst_buffer, rob;

void rob_management(inst_t * finished_inst, buffer_t * rob){
}

void choose_rs(inst_t * inst){
  int min = INT_MAX;
  int indice_res = 0;
  for(int j=0; j<get_numberofstations(inst); j++){
    if(get_size(inst->rs[j]->inst_buffer) < min){
      min = get_size(inst->rs[j]->inst_buffer);
      indice_res = j;
    }
  }
  insert_element(res_stations[indice_res]->inst_buffer,inst);
}

void manage_down_dependencies(inst_t * inst){
  int number_of_down_dep = inst->dep_down->get_size();
  for(int i=0; i<number_of_down_dep; i++){
    inst_t * inst_down = inst->dep_down->buffer[i];
    for(int j=0; j<inst_down->num_of_dep; j++){
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

int manage_own_dependency(inst_t * inst){
  return ((inst->actual_exec_latency==0) ? 0 : --(inst->actual_exec_latency));
}

int put_into_FU(reservation_station_t * res){
  int limite = res->inst_buffer->get_size();
  int del_index;
  for(int j=0; j < limite; j++){
    if((!res->is_occupied)&&(res->inst_buffer[j]->dep_to_solve <= 0)){
      res->inst_id = res->inst_buffer[j];
      del_index = j;
      break;
    } 
  }
  return del_index; 
}

void step(int issue_width, buffer_t * inst_buffer){
  
  int counter = 0;
  
  /* -----------------  PLACES INSTRUCTIONS INTO CORRESPONDING RESERVATION STATIONS -------- */
  
  for(int i = 0; i<issue_width; i++){
    inst_t * new_inst = buffer->remove();
    choose_rs(new_inst);
  }
  
  /*-----------------------STARTS TREATING EACH RESERVATION STATION--------------*/
  
  for(int i=0; i<NUMBER_OF_STATIONS; i++){
    
    //See's if there is one instruction ready to be executed in the queu
    //Return the index of the instruction selected out of the queu to be deleted after
    int index_to_delete = put_into_FU(res_stations[i]);
    
    //Update the latencies of the instructions that depends on the one being executed
    inst_t * current_inst = res_stations[i]->inst_id;
    manage_down_dependencies(current_inst);
    
    //Update own latency
    if(manage_own_dependency(current_inst) <= 0)
      current_inst->done=1;
    
    //ROB Management

    //Write to file

    //Delete from instruction buffer on RS
    res_stations[i]->inst_buffer->remove(del_index);  
  }
}

int main(char * argc, char * argv[]){

  FILE * fp;
  if((fp = fopen("/inputs/test1.txt","r")) == NULL){
    printf("Error opening file!\n");
    exit(1);
  }
  
  char str[MAXCHAR];
  int issue_width;
  if(fscanf(fp,"%s %d",str,issue_width)!=2)
    printf("Error reading issue width\n");
  
  int number_of_stations;
  int stations_sizes[number_of_stations];
  int rob_size;
  int number_of_instructions;
  
  if(fscanf(fp,"%s %d",str,number_of_stations)!=2)
    printf("Error reading number of stations\n");
  for(int i=0; i<number_of_stations; i++){
    if(fscanf(fp,"%s %d", str, station_sizes[i])!=2)
      printf("Error reading station sizes\n");
  }
  
  //Init reservation stations
  res_stations = (reservation_station_t**) malloc(number_of_stations*sizeof(reservation_station_t*))
  for(int i=0; i<number_of_stations; i++){
    res_stations[i]->init_res_stations(station_sizes[i]);
  }

  if(fscanf(fp,"%s %d", str, rob_size)!=2)
    printf("Error reading ROB SIZE\n");

  //Init rob
  buffer_t * rob = init_buffer(rob_size);
  
  if(fscanf(fp,"%s %d",str,number_of_instructions)!=2)
    printf("Error reading line INSTRUCTIONS!\n");

  char c_rs_vector_sizes[MAXCHAR];
  char c_latencies[MAXCHAR];
  
  //Instruction buffer workin as FIFO
  buffer_t * FIFO_buffer = init_buffer(SIZE_MAX_INSTRUCTION_BUFFER);
  for(int i=0; i<number_of_instructions; i++){
    fprintf(fp,"%s %s %s",str,c_rs_vector_sizes,c_latencies);
    //Instantiate instruction, its latencies and its pointers to reservation stations
    int size_rs = strlen(c_rs_vector_sizes);
    inst_t * inst = init_instruction(size_rs,c_latencies[0] - '0',i,c_rs_vector_sizes);
    FIFO_buffer->insert(inst);
  }
  
  //Dependency part
  //Step function
}
