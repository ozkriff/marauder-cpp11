// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__COLOR3U_HPP
#define VISUALIZER__COLOR3U_HPP

typedef unsigned char UnsignedByte;

class Color3u {
public:
  Color3u(UnsignedByte red, UnsignedByte green, UnsignedByte blue);
  Color3u(UnsignedByte n);
  Color3u(int red, int green, int blue);

  UnsignedByte red() const;
  UnsignedByte green() const;
  UnsignedByte blue() const;

private:
  UnsignedByte mRed, mGreen, mBlue;
};

#endif
