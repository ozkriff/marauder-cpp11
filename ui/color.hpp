// See LICENSE file for copyright and license details.

#ifndef UI__COLOR_HPP
#define UI__COLOR_HPP

class Color {
public:
  Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f);

  float red();
  float green();
  float blue();

private:
  float mRed, mGreen, mBlue;
};

#endif
