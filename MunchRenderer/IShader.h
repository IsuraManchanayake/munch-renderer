#pragma once

#include "Vertex.h"

struct Image;

struct VertShaderOutput {
  vec3f screenPos;
  VertShaderOutput();
  VertShaderOutput(const vec3f screenPos);
  VertShaderOutput(const vec4f screenPos);
};

struct FragShaderOutput {
  Color color;
  bool discard;
  FragShaderOutput();
  FragShaderOutput(const Color &color);
  FragShaderOutput(bool discard);
};

struct IShader {
  const Image *texture;
  size_t width;
  size_t height;
  std::array<Vertex, 3> tri;

  IShader();
  IShader(size_t width, size_t height);
  virtual ~IShader();

  virtual VertShaderOutput vert(const Vertex &v, size_t idx) = 0;
  virtual FragShaderOutput frag(float r0, float r1, float r2) = 0;

  void attachTexture(Image *texture);
  Vertex computeFragVert(float r0, float r1, float r2) const;
};

struct DullShader : IShader {
  DullShader();
  DullShader(size_t width, size_t height);
  virtual VertShaderOutput vert(const Vertex &v, size_t idx);
  virtual FragShaderOutput frag(float r0, float r1, float r2);
};