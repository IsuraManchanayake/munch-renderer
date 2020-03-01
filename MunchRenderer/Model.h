#pragma once

#include "Vertex.h"

#include <vector>
#include <fstream>
#include <string>
#include <sstream>

struct Model {
  Model(const char *filename);
  void load();

  void translate(const vec3f &v);
  void scale(const vec3f &v);
  void rotx(float a);
  void roty(float a);
  void rotz(float a);

  std::vector<vec3f> vs;
  std::vector<std::array<Vertex, 3>> trs;
  std::string filename;
};
