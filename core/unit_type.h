/* See LICENSE file for copyright and license details. */

#ifndef CORE_UNIT_TYPE_H
#define CORE_UNIT_TYPE_H

typedef enum {
  UNIT_TANK,
  UNIT_TRUCK,
  UNIT_COUNT
} Unit_type_id;

typedef struct {
  int range_of_vision;
  int action_points;
} Unit_type;

const Unit_type* get_unit_type(int id);
void init_unit_types();

#endif
