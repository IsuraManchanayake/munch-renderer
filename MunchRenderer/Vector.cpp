#include "Vector.h"

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

vec4f operator*(const vec4f &v, const Matrix &mat) {
  vec4f res{};
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      res.data[i] += v.data[j] * mat.data[i + j * 4];
    }
  }
  return res;
}

vec4f operator*(const vec3f &v, const Matrix &mat) {
  return vec4f{v.x, v.y, v.z, 1.f} * mat;
}

Matrix Matrix::translate(const vec3f &v) {
  return {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f, v.x, v.y, v.z, 1.f};
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
