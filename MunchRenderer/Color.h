#pragma once

#include <cinttypes>

#include "Math.h"

typedef uint8_t byte;

struct Color {
  union {
    struct {
      byte b;
      byte g;
      byte r;
      byte a;
    };
    uint32_t col;
    byte bgra[4];
  };

  Color(byte r, byte g, byte b, byte a = 255);
  Color(byte value, byte a = 255);
  Color();

  void set(byte r, byte g, byte b, byte a = 255);
  void sethsva(int16_t h, byte s, byte v, byte a);

  Color operator*(float intensity) const;
  friend Color operator*(float intensity, const Color &col);

  static Color lerp(const Color &col0, const Color &col1, float f);
  static Color bary(const Color &col0, const Color &col1, const Color &col2,
                    float r1, float r2, float r3);
};

namespace Col {
extern Color defaultBackground;
extern Color defaultStroke;
extern Color black;
extern Color red;
extern Color green;
extern Color blue;
extern Color cyan;
extern Color magenta;
extern Color yellow;
extern Color white;
} // namespace Col
