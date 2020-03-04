#pragma once

#include "Color.h"
#include "Common.h"
#include "Vector.h"
#include "Vertex.h"
#include "IShader.h"

#include <cinttypes>
#include <string>

class Renderer;
class Image;

enum class RenderMode { TextureRaster, SolidColorRaster, WireFrame };

class Canvas {
public:
  Canvas(size_t width, size_t height);
  virtual ~Canvas();

protected:
  Color &operator[](size_t pos);
  const Color &operator[](size_t pos) const;
  Color &get(size_t x, size_t y);
  const Color &get(size_t x, size_t y) const;

  void clear(const Color &color = Col::defaultBackground);
  void setFlipVertical(bool flipVertical);

  void line(const Vertex &vx0, const Vertex &vx1);
  void line(int x0, int y0, int x1, int y1, const Color &col0,
            const Color &col1);
  void triangle(const Vertex &vx0, const Vertex &vx1, const Vertex &vx2,
                IShader* shader);
  void triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                const Color &fill);
  void model(const std::wstring &name, IShader* shader);
  void image(const Image &image, const vec2i &pos, const vec2i &size);

  virtual void update() = 0;

private:
  void refresh();

protected:
  Color *data;
  float *zbuf;

  Renderer *renderer;
  RenderMode renderMode;

  bool flipVertical;
  bool drawBoundBox;

  size_t width;
  size_t height;

  float deltaTime;
  size_t deltaTimeNano;

  vec3f light;

private:
  size_t currentTImeNano;

  friend class Renderer;
};
