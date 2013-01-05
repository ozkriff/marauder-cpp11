// See LICENSE file for copyright and license details.

#include <unittest++/UnitTest++.h>
#include "core/los.h"

SUITE(coreLos) {
  TEST(ConstructorTest) {
    Los los(V2i(0, 0), V2i(0, 1));
  }
}
