// See LICENSE file for copyright and license details.

#include <unittest++/UnitTest++.h>
#include "core/lineOfSight.hpp"

SUITE(coreLineOfSIght) {
  TEST(ConstructorTest) {
    LineOfSight los(V2i(0, 0), V2i(0, 1));
  }
}
