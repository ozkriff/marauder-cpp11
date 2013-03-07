// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__COLOR_HPP
#define VISUALIZER__COLOR_HPP

class Color {
public:
  Color(float red, float green, float blue, float alpha = 1.0f);

  float red() const;
  float green() const;
  float blue() const;
  float alpha() const;

private:
  float mRed, mGreen, mBlue, mAlpha;
};

#endif
