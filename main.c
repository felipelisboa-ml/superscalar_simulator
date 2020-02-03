#include "inst.h"
#include "reservation_station.h"

#define SIZE_MAX_INSTRUCTION_BUFFER 16
#define SIZE_MAX_RESERVATION_BUFFER 8
#define NUMBER_OF_STATIONS 4

reservation_station_t ** res_stations;

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
      //PUT INTO ROB 
    }
    //ADD WRITE TO FILE
    //DELETE FROM INSTRUCTION BUFFER AFTER WRITING
    //ROB GESTION
  }
}

int main(){
  //Read file operations
  int number_of_stations = 0;
  
  buffer_t * inst_buffer;
  inst_buffer = init_buffer(SIZE_MAX_INSTRUCTION_BUFFER);
  
  //Alocate and put instructions into buffer
  //Build dependency vectors (and initialize them)
  //inst_t * inst_read = from file
  
  inst_buffer->insert(inst_read);
  
  //Init reservation stations
  res_stations = (reservation_station_t**) malloc(number_of_stations*sizeof(reservation_station_t))
  for(int i=0; i<number_of_stations; i++){
    res_stations[i]->init_res_stations(SIZE_MAX_RESERVATION_BUFFER);
  }

  
  //Step function
}
