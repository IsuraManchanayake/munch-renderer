#include "Color.h"
#include "Math.h"

Color defaultBackground{100, 100, 100};
Color defaultStroke{200, 0, 128};

Color Color::lerp(const Color &col0, const Color &col1, float f) {
  Color result;
  for (size_t i = 0; i < 4; i++) {
    result.c[i] = static_cast<byte>(::lerp(col0.c[i], col1.c[i], f));
  }
  return result;
}