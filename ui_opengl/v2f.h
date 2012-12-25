/* See LICENSE file for copyright and license details. */

class V2f {
private:
  float mX;
  float mY;

public:
  V2f(float x, float y);
  V2f();
  ~V2f();

  float x() const { return mX; }
  float y() const { return mY; }

  void setX(float x) { mX = x; }
  void setY(float y) { mY = y; }

  void rotate(V2f *p, float angle);
  V2f& operator+(const V2f& b);
};
