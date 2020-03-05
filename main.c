#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "inst.h"
#include "circ_buffer.h"
#include "reservation_station.h"

#define SIZE_MAX_INSTRUCTION_BUFFER 16
#define MAXITER 20

reservation_station_t ** res_stations;
buffer_t * general_buffer;
buffer_t * completed_instructions;
circ_buffer_t * rob;

void rob_management(){
  if(rob->circ_buffer[rob->head] != NULL && (rob->circ_buffer[(rob->head)+1]!= NULL)){
    if((rob->circ_buffer[rob->head]->done == 1) && (rob->circ_buffer[(rob->head)+1]->done == 1)){
      inst_t * inst1 = remove_element_circ(rob);
      inst_t * inst2 = remove_element_circ(rob);			
      printf("--------------------------------------- removed from rob I%d  & I%d\n",inst1->id,inst2->id);
    }
  }
}

void write_to_file(FILE * f_out, reservation_station_t * res){
  if(get_size(res->inst_buffer) == 0)
    fprintf(f_out,"\t--");
  else{
    fprintf(f_out,"\t");
    for(int k=0; k<get_size(res->inst_buffer); k++)
	fprintf(f_out,"I%d ",res->inst_buffer->buffer[k]->id);
  }
  fprintf(f_out,"\t");
  if(res->inst_id != NULL) fprintf(f_out,"I%d",res->inst_id->id);
  else fprintf(f_out,"--");
}

void step(int issue_width, int num_of_stations, FILE * f_out,buffer_t * inst_buffer){
  
  /* --------  TAKES INSTRUCTIONS FROM QUEU AND PLACE INTO CORRESPONDING RESERVATION STATIONS -------- */
  
  for(int i = 0; i<issue_width; i++){
    inst_t * new_inst = remove_element(general_buffer,0);
    insert_element_circ(rob,new_inst);
    if(new_inst!=NULL) choose_rs(new_inst);
  }
  
  /*-----------------------TREATS RESERVATION STATION ONE AT A TIME --------------*/

  /* DEBUGGING PURPOSES
  for(int i=0; i<num_of_stations; i++){
    printf("Instruction buffer from RS %d \n", res_stations[i]->id);
    print_buffer(res_stations[i]->inst_buffer);
  }
  */
  fprintf(f_out,"\t");
  for(int i=0; i<num_of_stations; i++){

    /* -------------- LOOKS INTO BUFFER INSIDE THE CURRENT RESERVATION STATION ------------ */
    printf("----TREATING RS %d ----- \n", res_stations[i]->id);
    int index_to_delete = put_into_FU(res_stations[i]);
    switch(index_to_delete){
    case -3: printf("RS %d QUEU IS EMPTY\n", res_stations[i]->id); break;
    case -2: printf("RS %d is OCCUPIED\n", res_stations[i]->id); break;
    case -1: printf("INSTRUCTIONS IN QUEU ARE NOT ELIGIBLE TO EXECUTE, STILL HAVE DEPS TO SOLVE\n"); break;
    default: break;
    }

    write_to_file(f_out,res_stations[i]);
    
    /* ------------------- IF THERE IS CURRENTLY A INSTRUCTION IN THE FU ---------- */
    if(res_stations[i]->inst_id != NULL){

      /* DEBUGING PURPOSES
      printf("Instruction %d is currently at FU of RS  %d\n", res_stations[i]->inst_id->id, res_stations[i]->id);
      print_buffer(res_stations[i]->inst_id->dep_down);
      */
      
      inst_t * current_inst = res_stations[i]->inst_id;

      /* -------------- UPDATES ITS OWN EXECUTION LATENCY AND ADD TO COMPLETION BUFFER IF IT'S DONE ----------------  */
      int act_inst_latency = manage_own_latency(current_inst);
      if(act_inst_latency <= 0){
	insert_element(completed_instructions,current_inst);
	res_stations[i]->inst_id = NULL;
	current_inst->done = 1;
      }

      /* ------- IF THE CURRENT INSTRUCTION HAS CHILDREN, UPDATE THEIR DEPENDENCY LATENCIES ---------- */
      if(current_inst->dep_down!=NULL){
	printf("BEFORE MANAGING DEPENDENCIES:\n");
	print_deps(current_inst);
	manage_down_dependencies(current_inst);
	printf("AFTER MANAGING DEPENDENCIES:\n");
	print_deps(current_inst);  
      }
      
      
      /* ------- DELETE FROM INTRUCTION FROM RS BUFFER ------ */
      if(index_to_delete >= 0){
	inst_t * deleted_instruction = remove_element(res_stations[i]->inst_buffer,index_to_delete);
	if(deleted_instruction != NULL)
	  printf("Instruction %d was deleted from RS %d instruction buffer \n", deleted_instruction->id, i);
      }
    }
      
  }
  /* ------- MANAGES THE ROB --------- */
  rob_management();
  printf("Rob size: %d\n", get_size_circ(rob));
  
  /* ------------ WRITES IN ROB's COLUMN ------- */
  fprintf(f_out,"\t");
  for(int i=rob->head;i<((rob->head+get_size_circ(rob))%(rob->size));i++){
    if(rob->circ_buffer[i] != NULL)
      fprintf(f_out,"I%d ",rob->circ_buffer[i]->id);
  }
  fprintf(f_out,"\n");
}

int main(int argc, char * argv[]){

  if(argc!=2) {printf("Wrong usage!\n"); return 0;}
  
  FILE * fp = NULL;
  if((fp = fopen(argv[1],"r")) == NULL){
    printf("Error opening file!\n");
    return -1;
  }

  FILE * f_out = NULL;
  if((f_out = fopen("outputs/output.txt","w")) == NULL){
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

  //printf("Issue width: %d\n", *issue_width);
  //printf("Number of stations: %d\n", *number_of_stations);
  
  int * stations_sizes[*number_of_stations];
  for(int i=0; i<(*number_of_stations); i++){
    stations_sizes[i] = (int*) malloc(sizeof(int));
    if(fscanf(fp,"%s %d", str, stations_sizes[i])!=2)
      printf("Error reading station sizes\n");
    //printf("Station %d size: %d \n", i,  *stations_sizes[i]);
  }
  
  //Init reservation stations
  res_stations = (reservation_station_t**) malloc((*number_of_stations)*sizeof(reservation_station_t*));
  for(int i=0; i<(*number_of_stations); i++)
    res_stations[i] = init_res_station(i,*stations_sizes[i]);

  //Init rob
  int * rob_size = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d", str, rob_size)!=2)
    printf("Error reading ROB SIZE\n");
  //printf("Rob size: %d\n", *rob_size);
  rob = init_buffer_circ(*rob_size);

  int * number_of_instructions = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d",str,number_of_instructions)!=2)
    printf("Error reading line INSTRUCTIONS!\n");
  //printf("Number of instructions: %d\n", *number_of_instructions);

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
    if(!insert_element(general_buffer,inst))
      //printf("Instruction %d was added to general buffer\n", inst->id);
      //else
      printf("Error adding Instruction %d to general buffer\n", inst->id);
  }

  /* To test if memory allocation worked out fine  */
  if(general_buffer->buffer == NULL)
    printf("Problem allocating memory\n");
  else{
    for(int i=0; i<general_buffer->last; i++){
      if(general_buffer->buffer[i]==NULL)
	printf("Problem allocating memory!\n");
    }
  }
  if(fscanf(fp,"%s",str)!=1)
    printf("Error reading dependencies header!\n");

  //printf("%s\n",str);
  char first_inst[MAXCHAR];
  char second_inst[MAXCHAR];
  int * lat_dep = (int*) malloc(sizeof(int));
  
  //Dependency part
  while(fscanf(fp,"%s %s %d",first_inst,second_inst,lat_dep)!=EOF){
    printf("%s %s %d\n",first_inst,second_inst,*lat_dep);
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
    config_dependencies(t1,t2,*lat_dep);
  }

  //Calculates the number of dependencies to solve for each instruction added to the buffer
  for(int i=0; i<get_size(general_buffer); i++){
    //int x = calculate_up_deps(general_buffer->buffer[i]);
    printf("Up Deps to solve from Inst %d: %d\n",general_buffer->buffer[i]->id,calculate_up_deps(general_buffer->buffer[i]));
  }

  /* ------- WRITE THE FIRST LINE IN THE OUTPUT FILE --------- */
  fprintf(f_out,"CLOCK CYCLE");
  for(int i=0; i<(*number_of_stations); i++){
    fprintf(f_out,"\tRS%d \tFU%d",i,i);
  }
  fprintf(f_out,"\tROB\n");
  /* -------------------------------------------------------------*/
  
  //Step function
  int completed = 0;
  int clock = 0;
  
  printf("Starting algorithm\n");
  
  completed_instructions = init_buffer(*number_of_instructions);

  while(!completed){
    if(clock >= MAXITER) break;
    printf("===================================\n");
    printf("CLOCK CYCLE %d\n",clock);
    fprintf(f_out,"%d",clock);
    //ends when all the instructions are in the completed list
    if(completed_instructions->last == completed_instructions->size){
      completed=1;
      printf("==================================\n");
      printf("Algorithm completed in %d cycles\n",clock-1);
      printf("==================================\n");
    }
    else step(*issue_width,*number_of_stations,f_out,general_buffer);
    clock++;
  }
  return 0;
}
