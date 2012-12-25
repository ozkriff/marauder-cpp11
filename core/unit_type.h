/* See LICENSE file for copyright and license details. */

#ifndef CORE_UNIT_TYPE_H
#define CORE_UNIT_TYPE_H

enum class Unit_type_id {
  UNIT_TANK,
  UNIT_TRUCK,
  UNIT_COUNT
};

struct Unit_type {
  int range_of_vision;
  int action_points;
};

const Unit_type& get_unit_type(int id);
void init_unit_types();

#endif
