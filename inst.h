#ifndef INST_H
#define INST_H

#include "reservation_station.h"
#include "buffer.h"

extern res_stations;

//The vector "rs" in the struct represents the set of RS that the instruction can ocupy,
//dep_up is the instruction above in the dependence graph
//dep_down is a vector of all directly dependant instructions of the graph

typedef struct{
  reservation_station_t ** rs;
  int initial_latency,actual_latency,initial_dep_latency,dep_latency;
  inst_t * dep_up;
  buffer_t * dep_down;
  int done;
} inst_t;

inst_t * init_instruction(int initial_lat, int initial_dep_lat, inst_t * up, buffer_t * down);

inst_t * config_dependencies();

int get_numberofstations();

#endif


