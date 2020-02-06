#include "inst.h"

/* Most of the initialization work is done while reading the file,
   so this function isn't called at any point
*/

inst_t * init_instruction(int initial_lat, int initial_dep_lat, inst * up, buffer_t * down){
  inst_t * t = (inst_t*) malloc(sizeof(inst_t));
  t->initial_latency = initial_lat;
  t->actual_latency = initial_lat;
  t->initial_dep_latency = initial_dep_lat;
  t->dep_latency = i
  t->dep_up = up;
  t->dep_down = down;
  t->done = 0;
  return t
}
