// See LICENSE file for copyright and license details.

#include <unittest++/UnitTest++.h>
#include "core/direction.hpp"

SUITE(coreDirection) {
  TEST(ConstructorTest) {
    Direction direction;
  }

  TEST(ConstructorTest2) {
    Direction direction(DirectionID::NE);
    CHECK(DirectionID::NE == direction.value());
  }

  TEST(someTest) {
    int n = 1;
    CHECK_EQUAL(n, 1);
  }
}
