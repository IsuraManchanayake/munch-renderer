#include "TGAImage.h"

Image::Image()
    : width(), height(), format(DXGI_FORMAT_UNKNOWN), rowPitch(), slicePitch(),
      good(false), data(nullptr) {}

Image::Image(const DirectX::Image *image)
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

Image::Image(Image &&img) noexcept
    : width(img.width), height(img.height), format(img.format),
      rowPitch(img.rowPitch), slicePitch(img.slicePitch), good(img.good),
      data(nullptr) {
  std::swap(this->data, img.data);
}

Image &Image::operator=(Image &&img) noexcept {
  this->width = img.width;
  this->height = img.height;
  this->format = img.format;
  this->rowPitch = img.rowPitch;
  this->slicePitch = img.slicePitch;
  this->good = img.good;
  std::swap(this->data, img.data);
  return *this;
}

Image &Image::operator=(const Image &img) noexcept {
  if (this == &img) {
    return *this;
  }
  width = img.width;
  height = img.height;
  format = img.format;
  rowPitch = img.rowPitch;
  slicePitch = img.slicePitch;
  good = img.good;
  if (data)
    delete[] data;
  data = new Color[img.width * img.height];
  std::memcpy(data, img.data, width * height * sizeof(Color));
  return *this;
}

Image::~Image() {
  if (data) {
    delete[] data;
  }
}

Color Image::uv(const vec2f &uvcoord) const {
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

Image Image::solidColor(size_t width, size_t height, const Color &color) {
  Image image;
  image.width = width;
  image.height = height;
  image.format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
  image.rowPitch = width * 4;
  image.slicePitch = width * height * 4;
  image.good = true;
  image.data = new Color[width * height];
  for (size_t i = 0; i < width * height; i++) {
    image.data[i] = color;
  }
  return image;
}

Image TGAImage::load(const std::wstring &fname) {
  DirectX::ScratchImage scratch;
  HRESULT hr = DirectX::LoadFromTGAFile(fname.c_str(), nullptr, scratch);
  if (FAILED(hr)) {
    return {};
  } else {
    const DirectX::Image *image = std::move(scratch.GetImage(0, 0, 0));
    return {image};
  }
}
