#pragma once

#include "Color.h"

#include <vector>

struct TGAImage {
  std::vector<std::vector<Color>> data;

  void load(const char *filename);
};