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

  std::vector<std::array<Vertex, 3>> trs;
  Image texture;
  std::wstring name;
  static std::unordered_map<std::wstring, Model *> modelCache;
};
