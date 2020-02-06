#include <stdio.h>
#include <string.h>
#include "inst.h"
#include "reservation_station.h"

//This parameters will be defined by the input file
#define SIZE_MAX_INSTRUCTION_BUFFER 16
#define SIZE_MAX_RESERVATION_BUFFER 8
#define NUMBER_OF_STATIONS 4
#define ISSUE_WIDTH 2 

reservation_station_t ** res_stations;
buffer_t * inst_buffer, rob;

void rob_management(inst_t * finished_inst, buffer_t * rob){
  if(rob->get_size() > ISSUE_WIDTH)
    printf("ROB is full!\n");
  else{
    rob->insert(finished_inst);
    return 1;
  }
}
void step(int issue_width, buffer_t * inst_buffer){
  
  int counter = 0;
  
  /* -----------------  PUTS INSTRUCTIONS INTO GENERAL INSTRUCTION BUFFER -------- */
  
  for(int i = 0; i<issue_width; i++){
    //Gets first instruction from Instruction buffer
    //Instruction fields are alocated after reading the file, in this point we consider they already have their memory
    inst_t * new_inst = buffer->remove();
    //Chooses the least busy station
    int min = INT_MAX;
    int indice_res = 0;
    for(int j = 0; j < new_inst->get_numberofstations(); j++){
      if(new_inst->rs[j]->inst_buffer->get_size() < min){
	min = new_inst->rs[j]->inst_buffer->get_size();
	indice_res = j;
      }
    }
    res_stations[indice_res]->inst_buffer->insert(new_inst); 
  }
  
  /*-----------------------STARTS TREATING EACH RESERVATION STATION--------------*/
  
  for(int i=0; i<NUMBER_OF_STATIONS; i++){
    int limite = res_stations[i]->inst_buffer->get_size();
    int del_index;
    for(int j=0; j < limite; j++){
      if((res_stations[i]->occupied==0)&&(res_stations[i]->inst_buffer[j]->dep_latency<=1)){
	res_stations[i]->inst_id = res_stations[i]->inst_buffer[j];
	del_index = j;
	res_stations[i]->occupied = 1;
	break;
      }
    }
    
    //Update the latencies of the instructions that depends on the one being executed
    inst_t * current_inst = res_stations[i]->inst_id;
    int number_of_dependencies = inst->dep_down->get_size();
    for(int k=0; k<number_of_dependencies; k++){
      inst_t * inst_up = res_stations[i]->inst_id->dep_up;
      inst_t * inst_down = res_stations[i]->inst_id->dep_down->buffer[k];
      if(current_inst->dep_latency == current_inst->initial_dep_latency)
	//Case where the instruction occuping the RS is being updated for the first time 
	inst_down->lat_dep = inst_down->lat_dep - (inst_up->initial_latency - inst_up->actual_latency);
      else
	inst_down->lat_dep--; 
    }
    //Update own latency
    current_inst->actual_latency--;
    if(current_inst->actual_latency==0){
      res_stations[i]->occupied=0;
      current_inst->done=1;
      if(rob_management(current_inst,rob))
    }  
    //ADD WRITE TO FILE
    //DELETE FROM RS INSTRUCTION BUFFER AFTER WRITING
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
  fscanf(fp,"%s %d",str,issue_width);
  
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
    res_stations[i]->init_res_stations(station_sizes[i],i);
  }

  if(fscanf(fp,"%s %d", str, rob_size)!=1)
    printf("Error reading ROB SIZE\n");

  //Init rob
  buffer_t * rob = init_buffer(rob_size);
  
  if(fscanf(fp,"%s %d",str,number_of_instructions)!=2)
    printf("Error reading line INSTRUCTIONS!\n");

  char c_rs_vector_sizes[MAXCHAR];
  char c_latencies[MAXCHAR];
  
  //Instruction buffer workin as FIFO
  buffer_t * inst_buffer = init_buffer(SIZE_MAX_INSTRUCTION_BUFFER);
  for(int i=0; i<number_of_instructions; i++){
    fprintf(fp,"%s %s %s",str,c_rs_vector_sizes,c_latencies);
    //Instantiate instruction, its latencies and its pointers to reservation stations
    int size_rs = strlen(c_rs_vector_sizes);
    inst_t * instruction = (inst_t*) malloc(sizeof(inst_t));
    instruction->initial_latency = (c_latencies - '0');
    instruction->actual_latency = instruction->initial_latency;
    instruction->rs = malloc(size_rs*sizeof(reservation_station_t*));
    for(int j=0; j<size_rs; j++){
      int ctoi = (c_rs_vector_sizes[j]-'0'); 
      switch(ctoi){
      case 0: (*instruction->rs[j]) = &res_stations[0]; break;
      case 1: (*instruction->rs[j]) = &res_stations[1]; break;
      case 2: (*instruction->rs[j]) = &res_stations[2]; break;
      case 3: (*instruction->rs[j]) = &res_stations[3]; break;
      default: (*instruction->rs[j]) = &res_stations[0]; break;
      } 
    }
    inst_buffer->insert(instruction);
  }
  
  //Dependency part
    
  //Step function
}
