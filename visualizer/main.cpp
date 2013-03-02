// See LICENSE file for copyright and license details.

#include "visualizer/visualizer.hpp"

int main(int ac, char** av) {
  UNUSED(ac);
  UNUSED(av);
  Core core;
  Visualizer visualizer(core);
  visualizer.run();
  return 0;
}
