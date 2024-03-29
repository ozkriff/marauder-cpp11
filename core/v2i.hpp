// See LICENSE file for copyright and license details.

#ifndef CORE__V2I_HPP
#define CORE__V2I_HPP

class V2i {
public:
  V2i(int x, int y);
  V2i();
  ~V2i();

  int x() const;
  int y() const;
  void setX(int x);
  void setY(int y);

  int distance(const V2i &b) const;

  bool operator==(const V2i& b) const;
  V2i operator-(const V2i& b) const;
  V2i operator-(int b) const;
  V2i operator+(const V2i& b) const;

  static int distance(const V2i& a, const V2i &b);

private:
  int mX;
  int mY;
};

#endif
