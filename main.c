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

/* Takes the number of commit_width instructions of the ROB if they're done */
void rob_management(int commit_width){
  inst_t * t = NULL;
  if((circ_buffer_size(rob) - commit_width) >= 0){
    int flag=0;
    for(int i=0; i<commit_width; i++){
      if(rob->buffer[rob->first+i] != NULL)
	if(!(rob->buffer[rob->first+i]->done == 1))
	  flag=1;
    }
    if(flag==0){
      for(int i=0; i<commit_width; i++)
	t = circ_buffer_get(rob);
    }
  }
  //Independent way: for last removal on ROB
  else{
    for(int i=0; i<circ_buffer_size(rob); i++){
      if(rob->buffer[rob->first] != NULL)
	if(rob->buffer[rob->first]->done == 1)
	  t = circ_buffer_get(rob);
    }
  }
}

/* Writes RESERVATION STATIONS & FUNCTIONAL UNITS status on output file
 * EXCEPT ROB, which is writen on the end of the step function
 */

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


void step(int issue_width, int commit_width,int num_of_stations, FILE * f_out){
  
  /* --------  TAKES INSTRUCTIONS FROM QUEU AND PLACE INTO CORRESPONDING RESERVATION STATIONS -------- */
  
  for(int i = 0; i<issue_width; i++){
    inst_t * new_inst = remove_element(general_buffer,0);
    if(new_inst != NULL){
      //Insets into ROB
      if(circ_buffer_put(rob,new_inst)==0) printf("ROB is FULL!\n");
      printf("ROB occupation after fetching from general buffer: %d\n",circ_buffer_size(rob));
      //Chooses a RS for the instruction (actual strategy is the first one possible)
      choose_rs(new_inst);
    }
  }

  fprintf(f_out,"\t");

  /*-----------------------TREATS RESERVATION STATION / FUNCTIONAL UNIT ONE AT A TIME --------------*/
  
  for(int i=0; i<num_of_stations; i++){

    /* -------------- LOOKS INTO BUFFER INSIDE THE CURRENT RESERVATION STATION ------------ */
    printf("\n----TREATING RS %d ----- \n", res_stations[i]->id);
    //put_into_FU function picks a instruction from RS queue (if there is any available) to put into FU
    int index_to_delete = put_into_FU(res_stations[i]);
    switch(index_to_delete){
    case -3: printf("RS %d QUEUE IS EMPTY\n", res_stations[i]->id); break;
    case -2: printf("RS %d is OCCUPIED\n", res_stations[i]->id); break;
    case -1: printf("INSTRUCTIONS IN QUEU ARE NOT ELIGIBLE TO EXECUTE, STILL HAVE DEPS TO SOLVE\n"); break;
    default: break;
    }

    /* --------------- WRITES CURRENT SITUATION TO OUTPUT FILE ------------------ */
    write_to_file(f_out,res_stations[i]);
    
    /* ------------------- IF THERE IS CURRENTLY A INSTRUCTION BEING EXECUTED IN THE FU ---------- */
    if(res_stations[i]->inst_id != NULL){

      inst_t * current_inst = res_stations[i]->inst_id;
      printf("Instruction %d is currently at corresponding FU\n", current_inst->id);

      /* -------------- UPDATES ITS OWN EXECUTION LATENCY, IF DONE, MARKS IT AS DONE AND ADD TO COMPLETION BUFFER  ----------------  */
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
      else printf("It has no down dependencies\n");
      
      /* ------- DELETE THE CHOSEN INSTRUCTION GOING TO FU FROM RS BUFFER ------ */
      if(index_to_delete >= 0){
	inst_t * deleted_instruction = remove_element(res_stations[i]->inst_buffer,index_to_delete);
	if(deleted_instruction != NULL)
	  printf("Instruction %d was deleted from RS %d instruction buffer \n", deleted_instruction->id, i);
      }
    }
      
  }
  
  /* ------- MANAGES THE ROB --------- */
  rob_management(commit_width);
  printf("Rob occupation after treating stations: %d\n",circ_buffer_size(rob));
  
  /* ------------ WRITES IN ROB's COLUMN TO OUTPUT FILE  ------- */
  fprintf(f_out,"\t");
  for(int i=rob->first; i<(rob->first+circ_buffer_size(rob))%(rob->max_size); i++){
    fprintf(f_out,"I%d ",rob->buffer[i]->id);
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

  int * commit_width = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d",str,commit_width)!=2)
    printf("Error reading commit width\n");
  
  int * number_of_stations = (int*) malloc(sizeof(int));
  if(fscanf(fp,"%s %d",str,number_of_stations)!=2)
    printf("Error reading number of stations\n");

  printf("Issue width: %d\n", *issue_width);
  printf("Commit width: %d\n", *commit_width);
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
  rob = circ_buffer_init(*rob_size);

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
    for(int i=0; i<general_buffer->last; i++){
      if(general_buffer->buffer[i]==NULL)
	printf("Problem allocating memory!\n");
    }
  }
  if(fscanf(fp,"%s",str)!=1)
    printf("Error reading dependencies header!\n");

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

  //Calculate and print the number of up dependencies to solve for each instruction added to the buffer
  for(int i=0; i<get_size(general_buffer); i++){
    printf("Up Deps to solve from Inst %d: %d\n",general_buffer->buffer[i]->id,calculate_up_deps(general_buffer->buffer[i]));
  }

  /* ------- WRITE THE FIRST LINE IN THE OUTPUT FILE --------- */
  fprintf(f_out,"CLOCK CYCLE");
  for(int i=0; i<(*number_of_stations); i++){
    fprintf(f_out,"\tRS%d \tFU%d",i,i);
  }
  fprintf(f_out,"\tROB\n");
  /* -------------------------------------------------------------*/

  completed_instructions = init_buffer(*number_of_instructions);
  
  /* --------------- STARTS ALGORITHM ---------------------------*/
  int completed = 0;
  int clock = 0;

  printf("Starting algorithm\n");
  
  while(!completed){
    if(clock >= MAXITER) break;
    printf("===================================\n");
    printf("CLOCK CYCLE %d\n",clock);
    //Ends when all the instructions are in the completed list
    if(completed_instructions->last == completed_instructions->size){
      completed=1;
      printf("==================================\n");
      printf("Algorithm completed in %d cycles\n",clock-1);
      printf("==================================\n");
    }
    else {
      fprintf(f_out,"%d",clock);
      step(*issue_width,*commit_width,*number_of_stations,f_out);
    }
    clock++;
  }
  return 0;
}

