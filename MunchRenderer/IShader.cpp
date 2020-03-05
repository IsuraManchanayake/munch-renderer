#include "IShader.h"
#include "TGAImage.h"

VertShaderOutput::VertShaderOutput() : screenPos() {}
VertShaderOutput::VertShaderOutput(const vec3f screenPos)
    : screenPos(screenPos) {}
VertShaderOutput::VertShaderOutput(const vec4f screenPos)
    : screenPos(screenPos.x, screenPos.y, screenPos.z) {}

FragShaderOutput::FragShaderOutput() : color(), discard(false) {}
FragShaderOutput::FragShaderOutput(const Color &color)
    : color(color), discard(false) {}
FragShaderOutput::FragShaderOutput(bool discard) : color(), discard(discard) {}

IShader::IShader() : texture(nullptr), width(), height(), tri() {}
IShader::IShader(size_t width, size_t height)
    : texture(nullptr), width(width), height(height), tri() {}
IShader::~IShader() {}

void IShader::attachTexture(Image *texture) { this->texture = texture; }
Vertex IShader::computeFragVert(float r0, float r1, float r2) const {
  return {
    r0 * tri[0].pos + r1 * tri[1].pos + r2 * tri[2].pos,
    r0 * tri[0].tex + r1 * tri[1].tex + r2 * tri[2].tex,
    r0 * tri[0].nrm + r1 * tri[1].nrm + r2 * tri[2].nrm
  };
}

DullShader::DullShader() : IShader() {}
DullShader::DullShader(size_t width, size_t height) : IShader(width, height) {}
VertShaderOutput DullShader::vert(const Vertex &v, size_t idx) {
  tri[idx] = v;
  return {v.pos};
}
FragShaderOutput DullShader::frag(float r0, float r1, float r2) {
  Vertex fragVert = computeFragVert(r0, r1, r2);
  float intensity = fragVert.nrm.normal().dot({0.f, 0.f, 1.f});
  return intensity * texture->uv(fragVert.tex);
};
