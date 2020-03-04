#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "inst.h"
#include "reservation_station.h"
#include "circ_buffer.h"

#define SIZE_MAX_INSTRUCTION_BUFFER 16

//This is a test for git branches

reservation_station_t ** res_stations;
buffer_t * general_buffer;
buffer_t * completed_instructions;
circ_buffer_t * rob;

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

void rob_management(inst_t * finished_inst){
}

void step(int issue_width, int num_of_stations, FILE* f_out,int number_of_instructions){

  /* --------  TAKES INSTRUCTIONS FROM QUEU AND PLACE INTO CORRESPONDING RESERVATION STATIONS -------- */

  for(int i = 0; i<issue_width; i++){
    inst_t * new_inst = remove_element(general_buffer,0);
    if(new_inst!=NULL) choose_rs(new_inst);
  }

  /*-----------------------TREATS RESERVATION STATION ONE AT A TIME --------------*/
  
  fprintf(f_out,"\t");
  
  for(int i=0; i<num_of_stations; i++){

    /* -------------- LOOKS INTO BUFFER INSIDE THE CURRENT RESERVATION STATION ------------ */
    int index_to_delete = put_into_FU(res_stations[i]);
    switch(index_to_delete){
    case -3: printf("RS %d QUEU IS EMPTY\n", res_stations[i]->id); break;
    case -2: printf("RS %d is OCCUPIED\n", res_stations[i]->id); break;
    case -1: printf("INSTRUCTIONS IN QUEU ARE NOT ELIGIBLE TO EXECUTE, STILL HAVE DEPS TO SOLVE\n"); break;
    default: break;
    }
    
    /* ------------------- IF THERE IS CURRENTLY A INSTRUCTION IN THE FU ---------- */
    if(res_stations[i]->inst_id != NULL){

      inst_t * current_inst = res_stations[i]->inst_id;

      /* -------------- UPDATES ITS OWN EXECUTION LATENCY AND ADD TO COMPLETION BUFFER IF IT'S DONE ----------------  */
      int act_inst_latency = manage_own_latency(current_inst);
      if(act_inst_latency <= 0){
        current_inst->done = 1;
      	insert_element(completed_instructions,current_inst);
      	//res_stations[i]->inst_id = NULL;
      }

      /* ------- IF THE CURRENT INSTRUCTION HAS CHILDREN, UPDATE THEIR DEPENDENCY LATENCIES ---------- */
      if(current_inst->dep_down!=NULL){
      	printf("BEFORE MANAGING DEPENDENCIES:\n");
      	print_deps(current_inst);
      	manage_down_dependencies(current_inst);
      	printf("AFTER MANAGING DEPENDENCIES:\n");
      	print_deps(current_inst);
      }

      /* ------------------------------------ MANAGES THE ROB ------------------------------------------------- */
      /*
      // adds finished instruction in the ROB
      int state;
      if(current_inst->done == 1){
        state = insert_element_circ(rob,current_inst);
        if(state == 1)
          res_stations[i]->inst_id = NULL;
      }
      // removes instructions from the ROB
      int cpt = rob->head;
      for(int l=0;l<(*number_of_instructions);l++){
	int cpt = rob->head;
	while(rob->circ_buffer[cpt] != NULL){
		if(rob->circ_buffer[cpt]->id == l)
			remove_element_circ(
		cpt = (cpt+1)%(rob->size);
	}
      }
      */   
    }
    
    /* -------- WRITES TO OUTPUT FILE ------------------- */
    write_to_file(f_out,res_stations[i]);
    
    /* ------- DELETE FROM INTRUCTION FROM RS BUFFER */
    if(index_to_delete >= 0 && res_stations[i]->inst_id != NULL){
      	inst_t * deleted_instruction = remove_element(res_stations[i]->inst_buffer,index_to_delete);
      	if(deleted_instruction != NULL)
      	  printf("Instruction %d was deleted from RS %d instruction buffer \n", deleted_instruction->id, i);
    }
  }

  /* WRITE TO ROB COLUMN */
  fprintf(f_out,"\t--\n");
  /*
  // writes in ROB's column
  char* s = "                    ";
  fprintf(f_out,"%s",s);
  if(rob->circ_buffer[rob->head] == NULL){
    fprintf(f_out,"--");
  }
  else{
    int cpt = rob->head;
    while(rob->circ_buffer[cpt] != NULL){
      fprintf(f_out,"I%d ",rob->circ_buffer[cpt]->id);
      cpt = (cpt+1)%(rob->size);
    }
  }
  fprintf(f_out,"\n");
  */
}

int main(int argc, char *argv[]){
  
  if(argc != 2) {printf("Wrong number of arguments!\n"); return 0;};

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
  circ_buffer_t * rob = init_buffer_circ(*rob_size);


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
    //printf("Dependency is from %d to %d and has value %d \n",t2->id, t1->id, *lat_dep);
    config_dependencies(t1,t2,*lat_dep);
    //Test's dep_down buffer from father
    //print_buffer(t1->dep_down);
    //Test's dep_up buffer from child
    //print_up_deps(t2);
    //printf("------------------------------\n");
  }

  //Calculates the number of dependencies to solve for each instruction added to the buffer
  for(int i=0; i<get_size(general_buffer); i++){
    printf("Up Deps to solve from Inst %d: %d\n",general_buffer->buffer[i]->id,calculate_up_deps(general_buffer->buffer[i]));
  }

  //Step function
  int completed = 0;
  int clock = 0;
  printf("==================================\n");
  printf("Starting algorithm\n");
  printf("==================================\n");

  /* ------- WRITE THE FIRST LINE IN THE OUTPUT FILE --------- */
  fprintf(f_out,"CLOCK CYCLE");
  for(int i=0; i<(*number_of_stations); i++){
    fprintf(f_out,"\tRS%d \tFU%d",i,i);
  }
  fprintf(f_out,"\tROB\n");
  /* -------------------------------------------------------------*/

  completed_instructions = init_buffer(*number_of_instructions);

  while(!completed){
    printf("CLOCK CYCLE %d\n",clock);
    fprintf(f_out,"%d",clock);
    //ends when all the instructions are in the completed list
    if(completed_instructions->last == completed_instructions->size){
      completed=1;
      printf("==================================\n");
      printf("Algorithm completed!\n");
      printf("==================================\n");
      fclose(fp);
      fclose(f_out);
    }
    else step(*issue_width,*number_of_stations,f_out,*number_of_instructions);
    clock++;
  }
}
