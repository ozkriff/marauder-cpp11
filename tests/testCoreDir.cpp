// See LICENSE file for copyright and license details.

#include <unittest++/UnitTest++.h>
#include "core/dir.h"

SUITE(core_dir) {
  TEST(ConstructorTest) {
    Dir dir;
  }

  TEST(ConstructorTest2) {
    Dir dir(DirID::NE);
    CHECK(DirID::NE == dir.value());
  }

  TEST(someTest) {
    int n = 1;
    CHECK_EQUAL(n, 1);
  }
}
