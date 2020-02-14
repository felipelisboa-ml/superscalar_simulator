#ifndef INST_H
#define INST_H

#include "reservation_station.h"
#include "buffer.h"

#define MAXCHAR 256

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

inst_t * init_instruction(int size_rs, int init_lat, int id, char vec_sizes[MAXCHAR]);

inst_t * config_dependencies();

//Don't know if I'm using these
int get_init_num_of_dep(inst_t * inst);

int get_numberofstations(inst_t * inst);



#endif


