#include "IShader.h"
#include "TGAImage.h"

// VertexShaderOutput //
VertShaderOutput::VertShaderOutput() : screenPos() {}
VertShaderOutput::VertShaderOutput(const vec3f screenPos)
    : screenPos(screenPos) {}
VertShaderOutput::VertShaderOutput(const vec4f screenPos)
    : screenPos(screenPos.x, screenPos.y, screenPos.z) {}

// FragShaderOutput //
FragShaderOutput::FragShaderOutput() : color(), discard(false) {}
FragShaderOutput::FragShaderOutput(const Color &color)
    : color(color), discard(false) {}
FragShaderOutput::FragShaderOutput(bool discard) : color(), discard(discard) {}

// IShader //
IShader::IShader()
    : texture(nullptr), normal(nullptr), width(), height(), triview(),
      triorig(), tbn() {}
IShader::IShader(size_t width, size_t height)
    : texture(nullptr), normal(nullptr), width(width), height(height),
      triview(), triorig(), tbn() {}
IShader::~IShader() {}
void IShader::attachTexture(Image *texture) { this->texture = texture; }
void IShader::attachNormal(Image *normal) { this->normal = normal; }
Vertex IShader::computeFragVert(float r0, float r1, float r2) const {
  return {r0 * triview[0].pos + r1 * triview[1].pos + r2 * triview[2].pos,
          r0 * triview[0].tex + r1 * triview[1].tex + r2 * triview[2].tex,
          r0 * triview[0].nrm + r1 * triview[1].nrm + r2 * triview[2].nrm,
          r0 * triview[0].tan + r1 * triview[1].tan + r2 * triview[2].tan};
}
//void IShader::computeTBN() {
//}

// DullShader //
DullShader::DullShader() : IShader() {}
DullShader::DullShader(size_t width, size_t height) : IShader(width, height) {}
VertShaderOutput DullShader::vert(const Vertex &v, size_t idx) {
  triview[idx] = v;
  return {v.pos};
}
FragShaderOutput DullShader::frag(float r0, float r1, float r2) {
  Vertex fragVert = computeFragVert(r0, r1, r2);
  float intensity = fragVert.nrm.normal().dot({0.f, 0.f, 1.f});
  return intensity * texture->uv(fragVert.tex);
};
