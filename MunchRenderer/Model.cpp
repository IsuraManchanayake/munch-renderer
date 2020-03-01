#include "Model.h"
#include "StringUtils.h"

#include <ios>
#include <limits>

Model::Model(const char *filename) : filename(filename) {}

void Model::load() {
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    return;
  }
  std::string s;
  std::vector<vec2f> vts;
  std::vector<vec3f> vns;
  while (ifs >> s) {
    if (s == "v") {
      vec3f v;
      ifs >> v.x >> v.y >> v.z;
      vs.emplace_back(v);
    } else if (s == "vt") {
      vec2f vt;
      ifs >> vt.x >> vt.y;
      ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      vts.emplace_back(vt);
    } else if (s == "vn") {
      vec3f vn;
      ifs >> vn.x >> vn.y >> vn.z;
      vns.emplace_back(vn);
    } else if (s == "f") {
      std::array<Vertex, 3> tr;
      for (size_t i = 0; i < 3; i++) {
        std::string ptn;
        ifs >> ptn;
        std::vector<std::string> splitted;
        StringUtils::split(splitted, ptn, '/');
        size_t pidx = StringUtils::lexical_cast<size_t>(splitted[0]) - 1;
        size_t tidx = StringUtils::lexical_cast<size_t>(splitted[1]) - 1;
        size_t nidx = StringUtils::lexical_cast<size_t>(splitted[2]) - 1;
        tr[i].pos = vs[pidx];
        tr[i].tex = vts[tidx];
        tr[i].nrm = vns[nidx];
        tr[i].col = {255, 255, 255, 255};
      }
      trs.push_back(tr);
    } else {
      continue;
    }
  }
}

void Model::translate(const vec3f &v) {
  for (auto &tr : trs) {
    for (auto &vx : tr) {
      vx.pos = vx.pos.translate(v);
    }
  }
}

void Model::scale(const vec3f &v) {
  for (auto &tr : trs) {
    for (auto &vx : tr) {
      vx.pos = vx.pos.scale(v);
    }
  }
}

void Model::rotx(float a) {}

void Model::roty(float a) {}

void Model::rotz(float a) {}
