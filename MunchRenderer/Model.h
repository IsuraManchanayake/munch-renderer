#pragma once

#include "TGAImage.h"
#include "Vertex.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Model {
  static Model* loadModel(const std::wstring& name);
  Model(std::wstring name);
  void loadobj();
  void loadtex();

  void translate(const vec3f &v);
  void scale(const vec3f &v);
  void rotx(float a);
  void roty(float a);
  void rotz(float a);

  std::vector<vec3f> vs;
  std::vector<std::array<Vertex, 3>> trs;
  Image texture;
  std::wstring name;
  static std::unordered_map<std::wstring, Model *> modelCache;
};
