#include "Color.h"
#include "Math.h"

Color defaultBackground{100, 100, 100};
Color defaultStroke{200, 0, 128};

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