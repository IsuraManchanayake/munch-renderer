#pragma once

#define NOMINMAX

#include "Color.h"
#include "DirectXTex.h"
#include "Math.h"
#include "Vector.h"

#include <sstream>
#include <string>
#include <vector>

#define max0(a, b) ((a) < (b) ? (b) : (a))

struct Image {
  size_t width;
  size_t height;
  DXGI_FORMAT format;
  size_t rowPitch;
  size_t slicePitch;
  bool good;
  Color *data;

  Image();
  Image(const DirectX::Image *image);
  Image(Image &&img) noexcept;
  Image &operator=(Image &&img) noexcept;
  Image &operator=(const Image &img) noexcept;
  ~Image();

  Color uv(const vec2f &uvcoord) const;

  static Image solidColor(size_t width, size_t height, const Color &color);
};

struct TGAImage {
  static Image load(const std::wstring &fname);
};
