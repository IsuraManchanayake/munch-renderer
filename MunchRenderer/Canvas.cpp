#include "Canvas.h"
#include "Math.h"
#include "Model.h"
#include "TGAImage.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

Canvas::Canvas(size_t width, size_t height)
    : data(nullptr), zbuf(nullptr), renderer(nullptr),
      renderMode(RenderMode::SolidColorRaster), flipVertical(true),
      drawBoundBox(false), width(width), height(height), deltaTime(0.f),
      deltaTimeNano(0),
      currentTImeNano(Timer<std::chrono::nanoseconds>::current()),
      light(0.f, 0.f, 1.f) {
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

void Canvas::setFlipVertical(bool flipVertical) {
  this->flipVertical = flipVertical;
}

void Canvas::line(const Vertex &vx0, const Vertex &vx1) {
  int x0 = static_cast<int>(vx0.pos.x);
  int y0 = static_cast<int>(vx0.pos.y);
  int x1 = static_cast<int>(vx1.pos.x);
  int y1 = static_cast<int>(vx1.pos.y);
  const Color &col0 = vx0.col;
  const Color &col1 = vx1.col;
  line(x0, y0, x1, y1, col0, col1);
}

void Canvas::line(int x0, int y0, int x1, int y1, const Color &col0,
                  const Color &col1) {
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
  float t = 0.f;
  float tinc = 1.f / (x1 - x0);
  for (int x = xmin; x <= xmax; x++) {
    int y = clamp<int>(y0 + (x - x0) * grad, 0, vheight - 1);
    Color col = Color::lerp(col0, col1, t += tinc);
    if (swapped) {
      data[y + x * width] = col;
    } else {
      data[x + y * width] = col;
    }
  }
}

void Canvas::triangle(const Vertex &vx0, const Vertex &vx1, const Vertex &vx2,
                      IShader *shader) {
  if (renderMode == RenderMode::WireFrame) {
    line(vx0, vx1);
    line(vx1, vx2);
    line(vx2, vx0);
  } else if (renderMode == RenderMode::SolidColorRaster ||
             renderMode == RenderMode::TextureRaster) {
    std::array<Vertex, 3> vxa = {vx0, vx1, vx2};
    std::array<VertShaderOutput, 3> sva; // Screen vertex array
    for (size_t idx = 0; idx < 3; idx++) {
      sva[idx] = shader->vert(vxa[idx], idx);
    }
    vec2i minb{width - 1, height - 1};
    vec2i maxb{0, 0};
    for (const auto &v : sva) {
      int x = v.screenPos.x;
      int y = v.screenPos.y;
      if (x < minb.x) {
        minb.x = std::max(x, 0);
      }
      if (y < minb.y) {
        minb.y = std::max(y, 0);
      }
      if (x > maxb.x) {
        maxb.x = std::min(x, (int)width - 1);
      }
      if (y > maxb.y) {
        maxb.y = std::min(y, (int)height - 1);
      }
    }
    if (drawBoundBox) {
      line(minb.x, minb.y, minb.x, maxb.y, {255, 0, 0}, {255, 0, 0});
      line(minb.x, minb.y, maxb.x, minb.y, {255, 0, 0}, {255, 0, 0});
      line(minb.x, maxb.y, maxb.x, maxb.y, {255, 0, 0}, {255, 0, 0});
      line(maxb.x, minb.y, maxb.x, maxb.y, {255, 0, 0}, {255, 0, 0});
    }
    for (size_t y = minb.y; y <= maxb.y; y++) {
      for (size_t x = minb.x; x <= maxb.x; x++) {
        vec3f p{x, y, 0.0};
        vec3f bcenter =
            p.bary(sva[0].screenPos, sva[1].screenPos, sva[2].screenPos);
        float r0 = bcenter.x;
        float r1 = bcenter.y;
        float r2 = bcenter.z;
        if (r0 >= 0 && r1 >= 0 && r2 >= 0) {
          float z = r0 * sva[0].screenPos.z + r1 * sva[1].screenPos.z +
                    r2 * sva[2].screenPos.z;
          if (zbuf[x + y * width] < z) {
            zbuf[x + y * width] = z;
            auto fragout = shader->frag(r0, r1, r2);
            if (!fragout.discard) {
              data[x + y * width] = fragout.color;
            }
          }
        }
      }
    }
  }
}

void Canvas::triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                      const Color &fill) {
  DullShader dullShader(width, height);
  Image image = Image::solidColor(1, 1, fill);
  dullShader.attachTexture(&image);
  triangle({{x0, y0, 0.f}, {}, {0.f, 0.f, 1.f}, fill},
           {{x0, y0, 0.f}, {}, {0.f, 0.f, 1.f}, fill},
           {{x0, y0, 0.f}, {}, {0.f, 0.f, 1.f}, fill}, &dullShader);
}

void Canvas::model(const std::wstring &name, IShader *shader) {
  Model *model = Model::loadModel(name);
  shader->attachTexture(&model->texture);
  shader->attachNormal(&model->normal);
  for (auto &tr : model->trs) {
    triangle(tr[0], tr[1], tr[2], shader);
  }
}

void Canvas::image(const Image &image, const vec2i &pos, const vec2i &size) {
  const size_t bminx = clamp<size_t>(pos.x, 0, width - 1);
  const size_t bminy = clamp<size_t>(pos.y, 0, height - 1);
  const size_t bmaxx =
      clamp<size_t>(static_cast<size_t>(pos.x) + size.x - 1, 0, width - 1);
  const size_t bmaxy =
      clamp<size_t>(static_cast<size_t>(pos.y) + size.y - 1, 0, height - 1);
  const size_t &imw = image.width;
  const size_t &imh = image.height;
  for (size_t y = bminy; y <= bmaxy; y++) {
    for (size_t x = bminx; x <= bmaxx; x++) {
      const float imx = (static_cast<float>(x - pos.x) * imw) / size.x;
      const float imy = (static_cast<float>(y - pos.y) * imh) / size.y;
      data[x + y * width] = image.uv({imx, imy});
    }
  }
}

void Canvas::refresh() {
  int64_t time = Timer<std::chrono::nanoseconds>::current();
  deltaTimeNano = time - currentTImeNano;
  deltaTime = deltaTimeNano / 1.e9f;
  currentTImeNano = time;
  update();
}