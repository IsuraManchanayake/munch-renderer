#pragma once

#include "Vertex.h"

struct Image;

struct VertShaderOutput {
  vec3f screenPos;
  VertShaderOutput();
  VertShaderOutput(const vec3f screenPos);
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

  IShader();
  IShader(size_t width, size_t height);
  virtual ~IShader();

  virtual VertShaderOutput vert(const Vertex &v) = 0;
  virtual FragShaderOutput frag(const Vertex &v) = 0;

  void attachTexture(Image *texture);
};

struct DullShader : IShader {
  DullShader();
  DullShader(size_t width, size_t height);
  virtual VertShaderOutput vert(const Vertex &v);
  virtual FragShaderOutput frag(const Vertex &v);
};