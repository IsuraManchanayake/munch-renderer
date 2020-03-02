#pragma once

#include "Color.h"
#include "DirectXTex.h"

#include <sstream>
#include <vector>

#define max0(a, b) ((a) < (b) ? (b) : (a))

struct Image {
  size_t width;
  size_t height;
  DXGI_FORMAT format;
  size_t rowPitch;
  size_t slicePitch;
  Color *data;
  bool good;

  Image()
      : width(), height(), format(DXGI_FORMAT_UNKNOWN), rowPitch(),
        slicePitch(), data(nullptr), good(false) {}

  Image(const DirectX::Image *image)
      : width(image->width), height(image->height), format(image->format),
        rowPitch(image->rowPitch), slicePitch(image->slicePitch), data(nullptr),
        good(true) {
    data = new Color[width * height];
    size_t chunksize = rowPitch / width;
    static const float factor = 256.f / 200;
    for (size_t i = 0, size = image->slicePitch; i < size; i += chunksize) {
      byte *p = image->pixels + i;
      byte *d = (byte *)(data + i / chunksize);
#if defined(TGA_COLOR_MAPPED)
      *(d + 0) = static_cast<byte>(factor * *(p + 2));
      *(d + 1) = static_cast<byte>(factor * *(p + 1));
      *(d + 2) = static_cast<byte>(factor * *(p + 0));
      *(d + 3) = chunksize == 4 ? static_cast<byte>(factor * *(p + 3)) : 255;
#else
      *(d + 0) = *(p + 2);
      *(d + 1) = *(p + 1);
      *(d + 2) = *(p + 0);
      *(d + 3) = chunksize == 4 ? *(p + 3) : 255;
#endif
    }
  }

  ~Image() {
    if (data) {
      delete[] data;
    }
  }
};

struct TGAImage {
  static Image load(const wchar_t *filename) {
    DirectX::ScratchImage scratch;
    HRESULT hr = DirectX::LoadFromTGAFile(filename, nullptr, scratch);
    if (FAILED(hr)) {
      return {};
    } else {
      std::stringstream ss;
      const DirectX::Image *image = std::move(scratch.GetImage(0, 0, 0));
      return {image};
    }
  }
};
