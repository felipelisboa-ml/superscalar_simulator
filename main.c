#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "inst.h"
#include "reservation_station.h"

//This parameters will be defined by the input file
#define SIZE_MAX_INSTRUCTION_BUFFER 16

reservation_station_t ** res_stations;
buffer_t * general_buffer, rob;
buffer_t * completed_instructions;

void rob_management(inst_t * finished_inst, buffer_t * rob){
}

void choose_rs(inst_t * inst){
  int min = INT_MAX;
  int indice_res = 0;
  printf("Instruction %d can be deployed to %d stations\n", inst->id, get_numberofstations(inst));
  //HAS TO ASSOCIATE THE RESERVATION STATION ON THE INSTRUCTION TO THE GLOBAL ONE
  insert_element(inst->rs[0]->inst_buffer,inst);
  printf("Inserted into RS %d\n", inst->rs[0]->id);
  
  /* STILL TO IMPLEMENT CHOOSE THE LEAST BUSY
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

int manage_own_dependency(inst_t * inst){
  return ((inst->actual_exec_latency==0) ? 0 : --(inst->actual_exec_latency));
}

int put_into_FU(reservation_station_t * res){
  int limite = res->inst_buffer->size;
  int del_index=0;
  for(int j=0; j < limite; j++){
    if((is_occupied(res)==NULL)&&(res->inst_buffer->buffer[j]->dep_to_solve <= 0)){
      res->inst_id = res->inst_buffer->buffer[j];
      del_index = j;
      break;
    } 
  }
  return del_index; 
}

void step(int issue_width, int num_of_stations, buffer_t * inst_buffer){
  
  int counter = 0;
  
  /* -----------------  PLACES INSTRUCTIONS INTO CORRESPONDING RESERVATION STATIONS -------- */
  
  for(int i = 0; i<issue_width; i++){
    inst_t * new_inst = remove_element(general_buffer,0); 
    choose_rs(new_inst);
  }
  
  /*-----------------------STARTS TREATING EACH RESERVATION STATION--------------*/
  for(int i=0; i<num_of_stations; i++){
    printf("Instruction buffer from RS %d \n", res_stations[i]->id);
    print_buffer(res_stations[i]->inst_buffer);
  }
  
  for(int i=0; i<num_of_stations; i++){

    printf("=============================\n");
    
    //See's if there is one instruction ready to be executed in the queu
    //Return the index of the instruction selected out of the queu to be deleted after
    int index_to_delete = put_into_FU(res_stations[i]);
    printf("Instruction %d was chosen to go into FU of RS  %d\n", res_stations[i]->inst_id->id, res_stations[i]->id);

    //Update the latencies of the instructions that depends on the one being executed
    inst_t * current_inst = res_stations[i]->inst_id;

    //print_buffer(res_stations[i]->inst_id->dep_down);
    
    if(current_inst->dep_down!=NULL){
      printf("BEFORE MANAGING DEPENDENCIES:\n");
      print_deps(current_inst);
      manage_down_dependencies(current_inst);
      printf("AFTER MANAGING DEPENDENCIES:\n");
      print_deps(current_inst);  
    }
    
    //Update own latency
    if(manage_own_dependency(current_inst) <= 0)
      insert_element(completed_instructions,current_inst);
      //current_inst->done=1;
    
    //ROB Management

    //Write to file

    //Delete from instruction buffer on RS
    inst_t * deleted_instruction = remove_element(res_stations[i]->inst_buffer,index_to_delete);
    if(deleted_instruction != NULL)
      printf("Instruction %d was deleted from RS %d instruction buffer \n", deleted_instruction->id, i);  
  }
}

int main(){

  FILE * fp = NULL;
  if((fp = fopen("inputs/test1.txt","r")) == NULL){
    printf("Error opening file!\n");
    return -1;
  }
  
  char str[MAXCHAR];
  int * issue_width = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d",str,issue_width)!=2)
    printf("Error reading issue width\n");
  
  int * number_of_stations = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d",str,number_of_stations)!=2)
    printf("Error reading number of stations\n");

  printf("Issue width: %d\n", *issue_width);
  printf("Number of stations: %d\n", *number_of_stations);
  
  int * stations_sizes[*number_of_stations];
  for(int i=0; i<(*number_of_stations); i++){
    stations_sizes[i] = (int*) malloc(sizeof(int));
    if(fscanf(fp,"%s %d", str, stations_sizes[i])!=2)
      printf("Error reading station sizes\n");
    printf("Station %d size: %d \n", i,  *stations_sizes[i]);
  }
  
  //Init reservation stations
  res_stations = (reservation_station_t**) malloc((*number_of_stations)*sizeof(reservation_station_t*));
  for(int i=0; i<(*number_of_stations); i++)
    res_stations[i] = init_res_station(i,*stations_sizes[i]);

  //Init rob
  int * rob_size = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d", str, rob_size)!=2)
    printf("Error reading ROB SIZE\n");
  printf("Rob size: %d\n", *rob_size);
  buffer_t * rob = init_buffer(*rob_size);

  int * number_of_instructions = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d",str,number_of_instructions)!=2)
    printf("Error reading line INSTRUCTIONS!\n");
  printf("Number of instructions: %d\n", *number_of_instructions);

  char c_rs_vector_sizes[MAXCHAR];
  char c_latencies[MAXCHAR];
  
  //Instruction buffer workin as FIFO
  general_buffer = init_buffer(SIZE_MAX_INSTRUCTION_BUFFER);
  for(int i=0; i<*number_of_instructions; i++){
    if(fscanf(fp,"%s %s %s",str,c_rs_vector_sizes,c_latencies)!=3)
      printf("Error reading instruction configuration!\n");
    //Instantiate instruction, its latencies and its pointers to reservation stations
    int size_rs = strlen(c_rs_vector_sizes);
    inst_t * inst = init_instruction(size_rs,c_latencies[0] - '0',i,c_rs_vector_sizes);
    if(insert_element(general_buffer,inst))
      printf("Instruction %d was added to general buffer\n", inst->id);
    else
      printf("Error adding Instruction %d to general buffer\n", inst->id);
  }

  /* To test if memory allocation worked out fine  */
  if(general_buffer->buffer == NULL)
    printf("Problem allocating memory\n");
  else{
    //printf("Estado do pointer last: %d\n", general_buffer->last);
    for(int i=0; i<general_buffer->last; i++){
      if(general_buffer->buffer[i]==NULL)
	printf("Problem allocating memory!\n");
    }
  }

  //int * number_of_deps = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s",str)!=1)
    printf("Error reading dependencies header!\n");

  printf("%s\n",str);
  char first_inst[MAXCHAR];
  char second_inst[MAXCHAR];
  int * lat_dep = (int*) malloc(sizeof(int));
  
  //Dependency part
  while(fscanf(fp,"%s %s %d",first_inst,second_inst,lat_dep)!=EOF){
    //printf("%s %s %d\n",first_inst,second_inst,*lat_dep);
    int first_id = first_inst[1] - '0';
    int secnd_id = second_inst[1] - '0';
    inst_t * t1 = NULL;
    inst_t * t2 = NULL;
    for(int i=0; i<get_size(general_buffer); i++){
      //find the two instructions on general buffer based on their ID (like a hash)
      if(general_buffer->buffer[i]->id == first_id)
	t1 = general_buffer->buffer[i];
      else if(general_buffer->buffer[i]->id == secnd_id)
	t2 = general_buffer->buffer[i];
    }
    //printf("Dependency is from %d to %d and has value %d \n",t2->id, t1->id, *lat_dep);
    config_dependencies(t1,t2,*lat_dep);
    //Test's dep_down buffer from father
    //print_buffer(t1->dep_down);
    //Test's dep_up buffer from child
    //print_up_deps(t2);
    //printf("------------------------------\n");
  }
  
  //Step function
  int completed = 0;
  int clock = 0;
  printf("==================================\n");
  printf("Starting algorithm\n");
  printf("==================================\n");

  completed_instructions = init_buffer(*number_of_instructions);
  //Change the condition, until all the instructions are done
  while(!completed){
    printf("CLOCK CYCLE %d\n",clock);
    //ends when all the instructions are in the completed list
    if(completed_instructions->last == completed_instructions->size)
      completed=1;
    step(*issue_width,*number_of_stations,general_buffer);
    clock++;
  }
  
}
