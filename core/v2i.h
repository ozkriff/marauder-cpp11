/* See LICENSE file for copyright and license details. */

#ifndef CORE_V2I_H
#define CORE_V2I_H

class V2i {
public:
  int x;
  int y;

public:
  V2i(int x, int y) {
    this->x = x;
    this->y = y;
  }

  V2i() {
    this->x = 0;
    this->y = 0;
  }

  ~V2i() {
  }

  bool operator==(const V2i& b) const {
    return x == b.x && y == b.y;
  }

  V2i operator-(const V2i& b) const {
    return V2i(x - b.x, y - b.y);
  }

  V2i operator+(const V2i& b) const {
    return V2i(x + b.x, y + b.y);
  }
};

#endif
