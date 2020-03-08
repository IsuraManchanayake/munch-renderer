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
  model->loadnrm();
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
  const std::wstring texfname = name + L"_diffuse.tga";
  texture = TGAImage::load(texfname);
  if (!texture.good) {
    texture = Image::solidColor(1, 1, Col::white);
  }
}

void Model::loadnrm() {
  const std::wstring nrmfname = name + L"_nm_tangent.tga";
  normal = TGAImage::load(nrmfname);
  if (!texture.good) {
    normal = Image::solidColor(1, 1, {127, 127, 255});
  }
  calctan();
}

void Model::calctan() {
  for (auto &tr : trs) {
    Vertex &vx0 = tr[0];
    Vertex &vx1 = tr[1];
    Vertex &vx2 = tr[2];

    const vec2f duv1 = vx1.tex - vx0.tex;
    const vec2f duv2 = vx2.tex - vx0.tex;
    const vec3f e1 = vx1.pos - vx0.pos;
    const vec3f e2 = vx2.pos - vx0.pos;

    const float rdet = 1.f / (duv1.u * duv2.v - duv2.u * duv1.v);
    vec3f tan{(duv2.v * e1.x - duv1.v * e2.x) * rdet,
              (duv2.v * e1.y - duv1.v * e2.y) * rdet,
              (duv2.v * e1.z - duv1.v * e2.z) * rdet};
    tan = tan.normal();
    vx0.nrm = vx0.nrm.normal();
    vx1.nrm = vx1.nrm.normal();
    vx2.nrm = vx2.nrm.normal();

    vx0.tan = (tan - tan.dot(vx0.nrm) * vx0.nrm).normal();
    vx1.tan = (tan - tan.dot(vx1.nrm) * vx1.nrm).normal();
    vx2.tan = (tan - tan.dot(vx2.nrm) * vx2.nrm).normal();
  }
}