#include "IShader.h"
#include "TGAImage.h"

VertShaderOutput::VertShaderOutput() : screenPos() {}
VertShaderOutput::VertShaderOutput(const vec3f screenPos)
    : screenPos(screenPos) {}

FragShaderOutput::FragShaderOutput() : color(), discard(false) {}
FragShaderOutput::FragShaderOutput(const Color &color)
    : color(color), discard(false) {}
FragShaderOutput::FragShaderOutput(bool discard) : color(), discard(discard) {}

IShader::IShader() : texture(nullptr), width(), height() {}
IShader::IShader(size_t width, size_t height)
    : texture(nullptr), width(width), height(height) {}
IShader::~IShader() {}

void IShader::attachTexture(Image *texture) { this->texture = texture; }

DullShader::DullShader() : IShader() {}
DullShader::DullShader(size_t width, size_t height) : IShader(width, height) {}
VertShaderOutput DullShader::vert(const Vertex &v) { return {v.pos}; }
FragShaderOutput DullShader::frag(const Vertex &v) {
  float intensity = v.nrm.normal().dot({0.f, 0.f, 1.f});
  return intensity * texture->uv(v.tex);
};
