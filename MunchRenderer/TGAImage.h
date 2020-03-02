#pragma once

#define NOMINMAX

#include "Color.h"
#include "DirectXTex.h"
#include "Math.h"
#include "Vector.h"

#include <string>
#include <sstream>
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

  Image()
      : width(), height(), format(DXGI_FORMAT_UNKNOWN), rowPitch(),
        slicePitch(), good(false), data(nullptr) {}

  Image(const DirectX::Image *image)
      : width(image->width), height(image->height), format(image->format),
        rowPitch(image->rowPitch), slicePitch(image->slicePitch), good(true),
        data(nullptr) {
    data = new Color[width * height];
    size_t chunksize = rowPitch / width;
    auto conv = [](byte b) -> byte {
      return static_cast<byte>(255.f * pow(b / 255.f, 1 / 2.2f));
    };
    for (size_t i = 0, size = image->slicePitch; i < size; i += chunksize) {
      byte *p = image->pixels + i;
      byte *d = (byte *)(data + i / chunksize);
      *(d + 0) = conv(*(p + 2));
      *(d + 1) = conv(*(p + 1));
      *(d + 2) = conv(*(p + 0));
      *(d + 3) = chunksize == 4 ? conv(*(p + 3)) : 255;
    }
  }

  Image(Image &&img) noexcept
      : width(img.width), height(img.height), format(img.format),
        rowPitch(img.rowPitch), slicePitch(img.slicePitch), good(good),
        data(nullptr) {
    std::swap(this->data, img.data);
  }

  Image &operator=(Image &&img) noexcept {
    this->width = img.width;
    this->height = img.height;
    this->format = img.format;
    this->rowPitch = img.rowPitch;
    this->slicePitch = img.slicePitch;
    this->good = img.good;
    std::swap(this->data, img.data);
    return *this;
  }

  Color uv(const vec2f &uvcoord) const {
    float imx = static_cast<float>(uvcoord.x) * width;
    float imy = static_cast<float>(1.f - uvcoord.y) * height;
    float imxfrac = fraction(imx);
    float imyfrac = fraction(imy);
    size_t imx0 = floor(imx);
    size_t imy0 = floor(imy);
    size_t imx1 = (imx0 == width - 1) ? imx0 : imx0 + 1;
    size_t imy1 = (imy0 == height - 1) ? imy0 : imy0 + 1;
    Color c00 = data[imx0 + imy0 * width];
    Color c10 = data[imx1 + imy0 * width];
    Color c01 = data[imx0 + imy1 * width];
    Color c11 = data[imx1 + imy1 * width];
    Color cxy{};
    for (size_t i = 0; i < 4; i++) {
      cxy.bgra[i] = lerp(lerp(c00.bgra[i], c10.bgra[i], imxfrac),
                         lerp(c01.bgra[i], c11.bgra[i], imxfrac), imyfrac);
    }
    return cxy;
  }

  ~Image() {
    if (data) {
      delete[] data;
    }
  }
};

struct TGAImage {
  static Image load(const std::wstring &fname) {
    DirectX::ScratchImage scratch;
    HRESULT hr = DirectX::LoadFromTGAFile(fname.c_str(), nullptr, scratch);
    if (FAILED(hr)) {
      return {};
    } else {
      std::stringstream ss;
      const DirectX::Image *image = std::move(scratch.GetImage(0, 0, 0));
      return {image};
    }
  }
};
