#ifndef INST_H
#define INST_H

#include "reservation_station.h"
#include "buffer.h"

#define MAXCHAR 256
#define MAXUPDEPS 8
#define MAXDOWNDEPS 8

//All structures are declared (even if it's redundant) before declaration in order to deal with
//cyclic dependant structs
typedef struct reservation_station_t reservation_station_t;
typedef struct buffer_t buffer_t;
typedef struct inst_t inst_t;

extern reservation_station_t ** res_stations;

typedef struct{
  int inst_id,dep_latency,init_dep_latency;
} pair_up_dependency;

struct inst_t{
  reservation_station_t ** rs;
  int initial_exec_latency,actual_exec_latency,num_of_dep,dep_to_solve,id,num_of_stations;
  pair_up_dependency ** dep_up;
  buffer_t * dep_down;
  int done;
};

/* Responsable for initialzing basic fields of the instruction structure and atributing
   possible reservation stations to each instruction
   However, it doesn't deal with the dependencies
 */
inst_t * init_instruction(int size_rs, int init_lat, int id, char vec_sizes[MAXCHAR]);

/* This function is used to configure the dependency atributes coming from the graph 
 */
void config_dependencies(inst_t * up_inst, inst_t * down_inst, int dep_val);

int calculate_up_deps(inst_t * inst);
  
/* DEBUGGING FUNCTIONS */
void show_host_rs(inst_t * inst);
void print_up_deps(inst_t * inst);
void print_deps(inst_t * inst);

/* SOME GETTERS */
int get_init_num_of_dep(inst_t * inst);
int get_numberofstations(inst_t * inst);



#endif


