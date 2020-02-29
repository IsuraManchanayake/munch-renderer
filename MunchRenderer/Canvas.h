#pragma once

#include "Vector.h"

#include <cinttypes>

class Renderer;

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
    byte c[4];
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
  void sethsva(byte h, byte s, byte v, byte a) {}
};

extern Color defaultBackground;
extern Color defaultStroke;

enum class RenderMode { Raster, WireFrame };

class Canvas {
public:
  Canvas(size_t width, size_t height);
  virtual ~Canvas();

  Color &operator[](size_t pos);
  const Color &operator[](size_t pos) const;
  Color &get(size_t x, size_t y);
  const Color &get(size_t x, size_t y) const;

  void clear(const Color &color = defaultBackground);

  void line(int x0, int y0, int x1, int y1,
            const Color &stroke = defaultStroke);
  void line(const vec2i &v0, const vec2i &v1,
            const Color &stroke = defaultStroke);
  void triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                const Color &fill, const Color &stroke = defaultStroke);
  void triangle(const vec2i &v0, const vec2i &v1, const vec2i &v2,
                const Color &fill, const Color &stroke = defaultStroke);

  virtual void update() = 0;

protected:
  Color *data;
  Renderer *renderer;
  RenderMode renderMode;
  size_t width;
  size_t height;
  friend class Renderer;
};
