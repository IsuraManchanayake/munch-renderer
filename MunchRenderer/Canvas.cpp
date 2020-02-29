#include "Canvas.h"

#include <algorithm>
#include <cmath>
#include <vector>

Color defaultBackground{100, 100, 100};
Color defaultStroke{200, 0, 128};

Canvas::Canvas(size_t width, size_t height)
    : data(nullptr), renderer(nullptr), renderMode(RenderMode::Raster),
      width(width), height(height) {
  data = new Color[width * height];
}

Canvas::~Canvas() { delete[] data; }

Color &Canvas::operator[](size_t pos) { return data[pos]; }

const Color &Canvas::operator[](size_t pos) const { return data[pos]; }

Color &Canvas::get(size_t x, size_t y) { return data[x + width * y]; }

const Color &Canvas::get(size_t x, size_t y) const {
  return data[x + width * y];
}

void Canvas::clear(const Color &color) {
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      data[x + y * width].set(color.r, color.g, color.b, color.a);
    }
  }
}

void Canvas::line(int x0, int y0, int x1, int y1, const Color &stroke) {
  int dx = std::abs(x0 - x1);
  int dy = std::abs(y0 - y1);
  if (dx == 0 && dy == 0) {
    return;
  }
  bool swapped = false;
  if (dy > dx) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    swapped = true;
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  float grad = static_cast<float>(y1 - y0) / (x1 - x0);
  for (int x = x0; x <= x1; x++) {
    int y = static_cast<int>(y0 + (x - x0) * grad);
    if (swapped) {
      data[x + y * width] = stroke;
    } else {
      data[y + x * width] = stroke;
    }
  }
}

void Canvas::line(const vec2i &v0, const vec2i &v1, const Color &stroke) {
  line(v0.x, v0.y, v1.x, v1.y, stroke);
}

void Canvas::triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                      const Color &fill, const Color &stroke) {
  switch (renderMode) {
  case RenderMode::WireFrame: {
    line(x0, y0, x1, y1, stroke);
    line(x1, y1, x2, y2, stroke);
    line(x2, y2, x0, y0, stroke);
    break;
  }
  case RenderMode::Raster: {
    vec2i v0{x0, y0};
    vec2i v1{x1, y1};
    vec2i v2{x2, y2};
    std::array<vec2i, 3> va{v0, v1, v2};
    std::sort(
        va.begin(), va.end(),
        [](const vec2i &vl, const vec2i &vr) -> bool { return vl.y > vr.y; });
    float x0 = static_cast<float>(va[0].x);
    float x1 = static_cast<float>(va[1].x);
    float x2 = static_cast<float>(va[2].x);
    float y0 = static_cast<float>(va[0].y);
    float y1 = static_cast<float>(va[1].y);
    float y2 = static_cast<float>(va[2].y);
    float t0 = (y1 - y2) / (y0 - y2);
    float x_ = x2 + t0 * (x0 - x2);
    for (int y = y1; y >= y2; y--) {
      float t1 = (y - y1) / (y2 - y1);
      float x1_ = x1 + t1 * (x2 - x1);
      float x2_ = x_ + t1 * (x2 - x_);
      line(x1_, y, x2_, y, fill);
    }
    for (int y = y1; y <= y0; y++) {
      float t1 = (y - y1) / (y0 - y1);
      float x1_ = x1 + t1 * (x0 - x1);
      float x2_ = x_ + t1 * (x0 - x_);
      line(x1_, y, x2_, y, fill);
    }
    break;
  }
  }
}

void Canvas::triangle(const vec2i &v0, const vec2i &v1, const vec2i &v2,
                      const Color &fill, const Color &stroke) {
  triangle(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, fill, defaultStroke);
}