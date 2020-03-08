#pragma once

#include "Color.h"
#include "Vector.h"

struct Vertex {
  vec3f pos;
  vec2f tex;
  vec3f nrm;
  vec3f tan;
  Color col;

  Vertex() : pos(), tex(), nrm(), tan(), col() {}
  Vertex(const vec3f &pos, const Color &col)
      : pos(pos), col(col), tex(), tan(), nrm() {}
  Vertex(const vec3f &pos, const vec2f &tex, const vec3f &nrm)
      : pos(pos), tex(tex), nrm(nrm), tan(), col() {}
  Vertex(const vec3f &pos, const vec2f &tex, const vec3f &nrm, const Color &col)
      : pos(pos), tex(tex), nrm(nrm), tan(), col(col) {}
  Vertex(const vec3f &pos, const vec2f &tex, const vec3f &nrm, const vec3f tan)
      : pos(pos), tex(tex), nrm(nrm), tan(tan), col() {}
  Vertex(const vec3f &pos, const vec2f &tex, const vec3f &nrm, const vec3f tan,
         const Color &col)
      : pos(pos), tex(tex), nrm(nrm), tan(tan), col(col) {}
};
