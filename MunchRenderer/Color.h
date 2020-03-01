#pragma once

#include <cinttypes>

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

  Color(byte r, byte g, byte b, byte a = 255) : r(r), g(g), b(b), a(a) {}
  Color(byte value, byte a = 255) : r(value), g(value), b(value), a(a) {}
  Color() : Color(0, 0, 0, 0) {}
  void set(byte r, byte g, byte b, byte a = 255) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
  }
  void sethsva(byte h, byte s, byte v, byte a) {
    // to implement
  }
  static Color lerp(const Color &col0, const Color &col1, float f);
  static Color bary(const Color &col0, const Color &col1, const Color &col2,
      float r1, float r2, float r3);
};

extern Color defaultBackground;
extern Color defaultStroke;
