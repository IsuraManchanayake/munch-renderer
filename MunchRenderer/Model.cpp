#include "Model.h"
#include "StringUtils.h"

#include <ios>
#include <limits>

std::unordered_map<std::wstring, Model *> Model::modelCache;

Model *Model::loadModel(const std::wstring &name) {
  auto ptr = modelCache.find(name);
  if (ptr != modelCache.end()) {
    return ptr->second;
  }
  Model *model = new Model(name);
  model->loadobj();
  model->loadtex();
  modelCache[name] = model;
  return model;
}

Model::Model(std::wstring name) : name(std::move(name)) {}

void Model::loadobj() {
  std::wstring objfname = name + L".obj";
  std::ifstream ifs(objfname);
  if (!ifs.is_open()) {
    return;
  }
  std::string s;
  std::vector<vec3f> vs;
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
      std::string line;
      std::getline(ifs, line);
      std::vector<std::string> pts;
      StringUtils::rtrim(line);
      StringUtils::split(pts, line, ' ');
      std::vector<Vertex> poly(pts.size() - 1);
      for (size_t i = 0; i < pts.size() - 1; i++) {
        std::string ptn = pts[i + 1];
        std::vector<std::string> splitted;
        StringUtils::split(splitted, ptn, '/');
        size_t pidx = StringUtils::lexical_cast<size_t>(splitted[0]);
        size_t tidx = StringUtils::lexical_cast<size_t>(splitted[1]);
        size_t nidx = StringUtils::lexical_cast<size_t>(splitted[2]);
        poly[i].pos = vs[pidx - 1];
        poly[i].tex = tidx > 0 ? vts[tidx - 1] : vec2f{};
        poly[i].nrm = nidx > 0 ? vns[nidx - 1] : vec3f{};
        poly[i].col = {255, 255, 255, 255};
      }
      for (size_t i = 0; i < poly.size() - 2; i++) {
        std::array<Vertex, 3> tr{poly[0], poly[i + 1], poly[i + 2]};
        trs.emplace_back(tr);
      }
    } else {
      continue;
    }
  }
}

void Model::loadtex() {
  std::wstring texfname = name + L"_diffuse.tga";
  texture = TGAImage::load(texfname);
  if (!texture.good) {
    texture = Image::solidColor(1, 1, Col::white);
  }
}

