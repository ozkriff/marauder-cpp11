/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include "unit_type.h"

static Unit_type unit_types[UNIT_COUNT];

static void init_tank(Unit_type *t){
  assert(t);
  t->range_of_vision = 4;
  t->action_points = 12;
}

static void init_truck(Unit_type *t){
  assert(t);
  t->range_of_vision = 6;
  t->action_points = 16;
}

const Unit_type* get_unit_type(int id) {
  assert(id >= 0 && id < UNIT_COUNT);
  return unit_types + id;
}

void init_unit_types(void) {
  init_tank(unit_types + UNIT_TANK);
  init_truck(unit_types + UNIT_TRUCK);
}
