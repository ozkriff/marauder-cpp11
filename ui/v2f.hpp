// See LICENSE file for copyright and license details.

#ifndef UI__V2F_HPP
#define UI__V2F_HPP

class V2f {
public:
  V2f(float x, float y);
  V2f();
  ~V2f();

  float x() const;
  float y() const;

  void setX(float x);
  void setY(float y);

  void rotate(V2f *p, float angle);
  V2f operator+(const V2f& b);
  V2f operator-(const V2f& b);
  V2f operator+(float b);
  V2f operator*(float b);
  V2f operator/(float b);

private:
  float mX;
  float mY;
};

#endif
