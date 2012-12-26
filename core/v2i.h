/* See LICENSE file for copyright and license details. */

#ifndef CORE_V2I_H
#define CORE_V2I_H

class V2i {
public:
  int x;
  int y;

public:
  V2i(int x, int y);
  V2i();
  ~V2i();

  bool operator==(const V2i& b) const;
  V2i operator-(const V2i& b) const;
  V2i operator+(const V2i& b) const;
};

#endif
