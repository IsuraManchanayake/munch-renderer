#include "Color.h"
#include "Math.h"

Color Col::defaultBackground{100, 100, 100};
Color Col::defaultStroke{200, 0, 128};
Color Col::black{0, 0, 0};
Color Col::red{255, 0, 0};
Color Col::green{0, 255, 0};
Color Col::blue{0, 0, 255};
Color Col::cyan{0, 255, 255};
Color Col::magenta{255, 0, 255};
Color Col::yellow{255, 255, 0};
Color Col::white{255, 255, 255};

Color::Color() : r(0), g(0), b(0), a(0) {}

Color::Color(byte r, byte g, byte b, byte a) : r(r), g(g), b(b), a(a) {}

Color::Color(byte value, byte a) : r(value), g(value), b(value), a(a) {}

void Color::set(byte r, byte g, byte b, byte a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
}

void Color::sethsva(int16_t h, byte s, byte v, byte a) {}

Color Color::operator*(float intensity) const {
  intensity = clamp<float>(intensity, 0.f, 1.f);
  Color res{};
  for (size_t i = 0; i < 4; i++) {
    res.bgra[i] = static_cast<byte>(bgra[i] * intensity);
  }
  return res;
}

Color operator*(float intensity, const Color &col) { return col * intensity; }

Color Color::lerp(const Color &col0, const Color &col1, float f) {
  Color result;
  for (size_t i = 0; i < 4; i++) {
    result.bgra[i] = static_cast<byte>(::lerp(col0.bgra[i], col1.bgra[i], f));
  }
  return result;
}

Color Color::bary(const Color &col0, const Color &col1, const Color &col2,
                  float r0, float r1, float r2) {
  return {static_cast<byte>(r0 * col0.r + r1 * col1.r + r2 * col2.r),
          static_cast<byte>(r0 * col0.g + r1 * col1.g + r2 * col2.g),
          static_cast<byte>(r0 * col0.b + r1 * col1.b + r2 * col2.b),
          static_cast<byte>(r0 * col0.a + r1 * col1.a + r2 * col2.a)};
}