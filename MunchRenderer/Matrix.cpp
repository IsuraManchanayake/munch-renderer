#include "Vector.h"

#include <algorithm>

Matrix::Matrix() : data() {}

template <typename... T>
Matrix::Matrix(T &&... init) : data({std::forward<T>(init)...}) {}

Matrix Matrix::transpose() const {
  Matrix res;
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      res.data[j + i * 4] = data[i + j * 4];
    }
  }
  return res;
}

bool Matrix::inv(Matrix &invmat) const {
  std::array<float, 16> mat = data;
  std::array<float, 16> &inv = invmat.data;
  for (size_t i = 0; i < 4; i++) {
    // inv[i + i * 4] = 1.f;
    inv[i * 5] = 1.f;
  }
  for (size_t iter = 0; iter < 4; iter++) {
    bool found = false;
    for (size_t i = iter; i < 4; i++) {
      // if (mat[i + i * 4] != 0) {
      if (mat[i * 5] != 0) {
        if (iter != i) {
          std::swap_ranges(std::begin(mat) + 4 * i, std::begin(mat) + 4 * i + 4,
                           std::begin(mat) + 4 * iter);
          std::swap_ranges(std::begin(inv) + 4 * i, std::begin(inv) + 4 * i + 4,
                           std::begin(inv) + 4 * iter);
        }
        found = true;
        break;
      }
    }
    if (!found)
      return false;
    // float coef = mat[iter + iter * 4];
    float coef = mat[iter * 5];
    for (size_t i = 0; i < 4; i++) {
      mat[i + iter * 4] /= coef;
      inv[i + iter * 4] /= coef;
    }
    for (size_t i = 0; i < 4; i++) {
      if (i == iter)
        continue;
      float coef = mat[iter + i * 4];
      if (coef != 0.f) {
        for (size_t j = 0; j < 4; j++) {
          mat[j + i * 4] -= mat[j + iter * 4] * coef;
          inv[j + i * 4] -= inv[j + iter * 4] * coef;
        }
      }
    }
  }
  return true;
}

Matrix Matrix::invtrans() const {
  Matrix invtr{};
  bool out = this->inv(invtr);
  if (out) {
    return invtr.transpose();
  } else {
    return {};
  }
}

float &Matrix::operator[](const size_t idx) { return data[idx]; }

const float &Matrix::operator[](const size_t idx) const { return data[idx]; }

Matrix Matrix::operator*(const Matrix &mat) const {
  Matrix res{};
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      for (size_t k = 0; k < 4; k++) {
        res.data[j + i * 4] += data[k + i * 4] * mat.data[j + k * 4];
      }
    }
  }
  return res;
}

vec4f Matrix::operator*(const vec4f &v) const {
  vec4f res{};
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      res.data[i] += this->data[j + i * 4] * v[j];
    }
  }
  return res;
}

vec4f Matrix::operator*(const vec3f &v) const {
  return this->operator*(vec4f{v.x, v.y, v.z, 1.f});
}

Matrix Matrix::translate(const vec3f &v) {
  return {1.f, 0.f, 0.f, v.x, 0.f, 1.f, 0.f, v.y,
          0.f, 0.f, 1.f, v.z, 0.f, 0.f, 0.f, 1.f};
}

Matrix Matrix::scale(const vec3f &v) {
  return {v.x, 0.f, 0.f, 0.f, 0.f, v.y, 0.f, 0.f,
          0.f, 0.f, v.z, 0.f, 0.f, 0.f, 0.f, 1.f};
}

Matrix Matrix::rotate(const vec3f &v) {
  const float ca = cos(v.x);
  const float sa = sin(v.x);
  const float cb = cos(v.y);
  const float sb = sin(v.y);
  const float cc = cos(v.z);
  const float sc = sin(v.z);
  return {ca * cb,
          ca * sb * sc - sa * cc,
          ca * sb * cc + sa * sc,
          0.f,
          sa * cb,
          sa * sb * sc + ca * cc,
          sa * sb * cc - ca * sc,
          0.f,
          -sb,
          cb * sc,
          cb * cc,
          0.f,
          0.f,
          0.f,
          0.f,
          1.f};
}

Matrix Matrix::camera(float z) {
  const float dist = 1.3;
  return {1.f/dist, 0.f, 0.f, 0.f, 0.f, 1.f/dist, 0.f,      0.f,
          0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f / z, 1.f};
}

Matrix Matrix::lookat(const vec3f &center, const vec3f &eye, const vec3f &up) {
  const vec3f camera = eye - center;
  const vec3f zz = camera.normal();
  const vec3f xx = up.cross(zz).normal();
  const vec3f yy = zz.cross(xx).normal();
  return {xx.x, xx.y, xx.z, 0.f, yy.x, yy.y, yy.z, 0.f,
          zz.x, zz.y, zz.z, 0.f, 0.f,  0.f,  0.f,  1.f};
}

Matrix Matrix::view(const vec3f& center, const vec3f& eye, const vec3f& up) {
  return lookat(center, eye, up) * Matrix::translate(-center);
}
