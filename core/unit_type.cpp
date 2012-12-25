/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include "core/unit_type.h"

static Unit_type unit_types[(int)Unit_type_id::UNIT_COUNT];

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
  assert(id >= 0 && id < (int)Unit_type_id::UNIT_COUNT);
  return unit_types + id;
}

void init_unit_types() {
  init_tank(unit_types + (int)Unit_type_id::UNIT_TANK);
  init_truck(unit_types + (int)Unit_type_id::UNIT_TRUCK);
}
