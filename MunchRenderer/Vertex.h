#pragma once

#include "Color.h"
#include "Vector.h"

struct Vertex {
  vec3f pos;
  vec2f tex;
  vec3f nrm;
  Color col;

  Vertex() : pos(), tex(), nrm(), col() {}
  Vertex(const vec3f &pos, const Color &col)
      : pos(pos), col(col), tex(), nrm() {}
  Vertex(const vec3f &pos, const vec2f &tex, const vec3f &nrm)
      : pos(pos), tex(tex), nrm(nrm), col() {}
};
