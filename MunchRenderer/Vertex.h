#pragma once

#include "Vector.h"
#include "Color.h"

struct Vertex {
  vec3f pos;
  Color col;

  Vertex(const vec3f &pos, const Color &col) : pos(pos), col(col) {}
};
