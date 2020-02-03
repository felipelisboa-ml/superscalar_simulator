#ifndef INST_H
#define INST_H

#include "reservation_station.h"
#include "buffer.h"

typedef struct{
  reservation_station_t ** rs;
  int initial_latency,actual_latency,initial_dep_latency,dep_latency;
  inst_t * dep_up;
  buffer_t * dep_down;
} inst_t;

inst_t * init_instruction();

int get_numberofstations();

#endif


