#pragma once

#include "Color.h"
#include "Common.h"
#include "Vertex.h"
#include "Vector.h"

#include <cinttypes>

class Renderer;

enum class RenderMode { Raster, WireFrame };

class Canvas {
public:
  Canvas(size_t width, size_t height);
  virtual ~Canvas();

protected:
  Color &operator[](size_t pos);
  const Color &operator[](size_t pos) const;
  Color &get(size_t x, size_t y);
  const Color &get(size_t x, size_t y) const;

  void clear(const Color &color = defaultBackground);

  void line(const Vertex &vx0, const Vertex &vx1);
  void line(int x0, int y0, int x1, int y1,
            const Color &stroke = defaultStroke);
  void line(const vec2i &v0, const vec2i &v1,
            const Color &stroke = defaultStroke);
  void triangle(const Vertex &vx0, const Vertex &vx1, const Vertex &vx2);
  void triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                const Color &fill, const Color &stroke = defaultStroke);
  void triangle(const vec2i &v0, const vec2i &v1, const vec2i &v2,
                const Color &fill, const Color &stroke = defaultStroke);

  virtual void update() = 0;

private:
  void refresh();

protected:
  Color *data;
  Renderer *renderer;
  RenderMode renderMode;
  size_t width;
  size_t height;
  float deltaTime;
  size_t deltaTimeNano;

private:
  size_t currentTImeNano;

  friend class Renderer;
};
