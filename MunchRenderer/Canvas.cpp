#include "Canvas.h"
#include "Math.h"
#include "Model.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

Canvas::Canvas(size_t width, size_t height)
    : data(nullptr), zbuf(nullptr), renderer(nullptr),
      renderMode(RenderMode::Raster), width(width), height(height),
      deltaTime(0.f), deltaTimeNano(0),
      currentTImeNano(Timer<std::chrono::nanoseconds>::current()) {
  data = new Color[width * height];
  zbuf = new float[width * height];
}

Canvas::~Canvas() {
  delete[] data;
  delete[] zbuf;
}

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
      zbuf[x + y * width] = -std::numeric_limits<float>::infinity();
    }
  }
}

void Canvas::line(const Vertex &vx0, const Vertex &vx1) {
  int x0 = static_cast<int>(vx0.pos.x);
  int y0 = static_cast<int>(vx0.pos.y);
  int x1 = static_cast<int>(vx1.pos.x);
  int y1 = static_cast<int>(vx1.pos.y);
  int dx = std::abs(x0 - x1);
  int dy = std::abs(y0 - y1);
  Color col0 = vx0.col;
  Color col1 = vx1.col;
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
    std::swap(col0, col1);
  }
  float grad = static_cast<float>(y1 - y0) / (x1 - x0);
  int vwidth = swapped ? height : width;
  int vheight = swapped ? width : height;
  int xmin = clamp<int>(x0, 0, vwidth - 1);
  int xmax = clamp<int>(x1, 0, vwidth - 1);
  for (int x = xmin; x <= xmax; x++) {
    int y = clamp<int>(y0 + (x - x0) * grad, 0, vheight - 1);
    float t = norm(x, x0, x1);
    Color col = Color::lerp(col0, col1, t);
    if (swapped) {
      data[y + x * width] = col;
    } else {
      data[x + y * width] = col;
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
  int vwidth = swapped ? height : width;
  int vheight = swapped ? width : height;
  int xmin = clamp<int>(x0, 0, vwidth - 1);
  int xmax = clamp<int>(x1, 0, vwidth - 1);
  for (int x = xmin; x <= xmax; x++) {
    int y = clamp<int>(y0 + (x - x0) * grad, 0, vheight - 1);
    if (swapped) {
      data[y + x * width] = stroke;
    } else {
      data[x + y * width] = stroke;
    }
  }
}

void Canvas::line(const vec2i &v0, const vec2i &v1, const Color &stroke) {
  line(v0.x, v0.y, v1.x, v1.y, stroke);
}

void Canvas::triangle(const Vertex &vx0, const Vertex &vx1, const Vertex &vx2) {
  switch (renderMode) {
  case RenderMode::WireFrame: {
    line(vx0, vx1);
    line(vx1, vx2);
    line(vx2, vx0);
    break;
  }
  case RenderMode::Raster: {
    std::array<Vertex, 3> vxa{vx0, vx1, vx2};
#if 0
    std::sort(vxa.begin(), vxa.end(),
              [](const Vertex &vxl, const Vertex &vxr) -> bool {
                return vxl.pos.y > vxr.pos.y;
              });
    float x0 = static_cast<float>(vxa[0].pos.x);
    float x1 = static_cast<float>(vxa[1].pos.x);
    float x2 = static_cast<float>(vxa[2].pos.x);
    float y0 = static_cast<float>(vxa[0].pos.y);
    float y1 = static_cast<float>(vxa[1].pos.y);
    float y2 = static_cast<float>(vxa[2].pos.y);
    Color &col0 = vxa[0].col;
    Color &col1 = vxa[1].col;
    Color &col2 = vxa[2].col;
    float t0 = norm(y1, y2, y0);
    float x_ = lerp(x2, x0, t0);
    Color col_ = Color::lerp(vxa[2].col, vxa[0].col, t0);
    for (int y = y1; y >= y2; y--) {
      float t1 = norm(y, y1, y2);
      float x1_ = lerp(x1, x2, t1);
      float x2_ = lerp(x_, x2, t1);
      Color col1_ = Color::lerp(col1, col2, t1);
      Color col2_ = Color::lerp(col_, col2, t1);
      Vertex vx1_{{x1_, y, 0.f}, col1_};
      Vertex vx2_{{x2_, y, 0.f}, col2_};
      line(vx1_, vx2_);
    }
    for (int y = y1 + 1; y <= y0; y++) {
      float t1 = norm(y, y0, y1);
      float x1_ = lerp(x0, x1, t1);
      float x2_ = lerp(x0, x_, t1);
      Color col1_ = Color::lerp(col0, col1, t1);
      Color col2_ = Color::lerp(col0, col_, t1);
      Vertex vx1_{{x1_, y, 0.f}, col1_};
      Vertex vx2_{{x2_, y, 0.f}, col2_};
      line(vx1_, vx2_);
    }
#endif
    vec2i minb{width - 1, height - 1};
    vec2i maxb{0, 0};
    for (const auto &v : vxa) {
      minb.x = std::min(minb.x, static_cast<int>(v.pos.x));
      minb.y = std::min(minb.y, static_cast<int>(v.pos.y));
      maxb.x = std::max(maxb.x, static_cast<int>(v.pos.x));
      maxb.y = std::max(maxb.y, static_cast<int>(v.pos.y));
    }
    // line(minb.x, minb.y, minb.x, maxb.y, {255, 0, 0});
    // line(minb.x, minb.y, maxb.x, minb.y, {255, 0, 0});
    // line(minb.x, maxb.y, maxb.x, maxb.y, {255, 0, 0});
    // line(maxb.x, minb.y, maxb.x, maxb.y, {255, 0, 0});
    for (size_t y = minb.y; y <= maxb.y; y++) {
      for (size_t x = minb.x; x <= maxb.x; x++) {
        vec3f p{x, y, 0.0};
        vec3f bcenter = p.bary(vx0.pos, vx1.pos, vx2.pos);
        float r0 = bcenter.x;
        float r1 = bcenter.y;
        float r2 = bcenter.z;
        if (r0 >= 0 && r1 >= 0 && r2 >= 0) {
          float z = r0 * vx0.pos.z + r1 * vx1.pos.z + r2 * vx2.pos.z;
          if (zbuf[x + y * width] < z) {
            zbuf[x + y * width] = z;
            data[x + y * width] =
                Color::bary(vx0.col, vx1.col, vx2.col, r0, r1, r2);
          }
        }
      }
    }
    break;
  }
  }
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

void Canvas::model(const char *filename) {
  Model model(filename);
  model.load();
  // renderMode = RenderMode::WireFrame;
  vec3f light{0.f, 0.f, 1.f};
  for (auto &tr : model.trs) {
    bool draw = true;
    for (size_t i = 0; i < 3; i++) {
      tr[i].pos =
          tr[i].pos.scale({300.f, 300.f, 300.f}).translate({400.f, 300.f, 0.f});
      float value = tr[i].nrm.normal().dot(light);
      /*if (value < 0.f) {
        draw = false;
        break;
      }*/
      value = map(value, 0.f, 1.f, 0.f, 256.f);
      byte intensity = static_cast<byte>(value);
      tr[i].col = {intensity, intensity, intensity, 255};
    }
    // if (draw) {
    triangle(tr[0], tr[1], tr[2]);
    //}
  }
}

void Canvas::refresh() {
  int64_t time = Timer<std::chrono::nanoseconds>::current();
  deltaTimeNano = time - currentTImeNano;
  deltaTime = deltaTimeNano / 1.e9f;
  currentTImeNano = time;
  update();
}