#pragma once

#include "Vector.h"

template <size_t N, template <size_t> typename MatrixO> struct MatrixImpl {
  std::array<float, N * N> data;

  MatrixImpl() : data() {}
  template <typename... T>
  MatrixImpl(T &&... args) : data{{std::forward<T>(args)...}} {}

  MatrixO<N> transpose() const;
  bool inv(MatrixO<N> &invmat) const;
  MatrixO<N> invtrans() const;

  float &operator[](const size_t idx);
  const float &operator[](const size_t idx) const;

  MatrixO<N> operator*(const MatrixO<N> &mat) const;
  vecnf<N> operator*(const vecnf<N> &v) const;
};

template <size_t N> struct Matrix : MatrixImpl<N, Matrix> {
  Matrix() : MatrixImpl<N, Matrix>() {}
  template <typename... T>
  Matrix(T &&... args) : MatrixImpl<N, Matrix>(std::forward<T>(args)...) {}

  using MatrixImpl<N, Matrix>::operator*;
};

template <> struct Matrix<4> : MatrixImpl<4, Matrix> {
  Matrix() : MatrixImpl<4, Matrix>() {}
  template <typename... T>
  Matrix(T &&... args) : MatrixImpl<4, Matrix>(std::forward<T>(args)...) {}

  vec4f operator*(const vec3f &v) const;

  static Matrix<4> translate(const vec3f &v);
  static Matrix<4> scale(const vec3f &v);
  static Matrix<4> rotate(const vec3f &v);
  static Matrix<4> camera(float z);
  static Matrix<4> lookat(const vec3f &center, const vec3f &eye,
                          const vec3f &up);
  static Matrix<4> view(const vec3f &center, const vec3f &eye, const vec3f &up);

  using MatrixImpl<4, Matrix>::operator*;
};

using mat3f = Matrix<3>;
using mat4f = Matrix<4>;

#define TEMPLATE_MATRIX_IMPL_HEADER                                            \
  template <size_t N, template <size_t> typename MatrixO>

TEMPLATE_MATRIX_IMPL_HEADER
MatrixO<N> MatrixImpl<N, MatrixO>::transpose() const {
  MatrixO<N> res;
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      res.data[j + i * N] = data[i + j * N];
    }
  }
  return res;
}

TEMPLATE_MATRIX_IMPL_HEADER
bool MatrixImpl<N, MatrixO>::inv(MatrixO<N> &invmat) const {
  auto mat = data;
  auto &inv = invmat.data;
  for (size_t i = 0; i < N; i++) {
    // inv[i + i * N] = 1.f;
    inv[i * (1 + N)] = 1.f;
  }
  for (size_t iter = 0; iter < N; iter++) {
    bool found = false;
    for (size_t i = iter; i < N; i++) {
      // if (mat[i + i * N] != 0) {
      if (mat[i * (1 + N)] != 0) {
        if (iter != i) {
          std::swap_ranges(std::begin(mat) + N * i, std::begin(mat) + N * i + N,
                           std::begin(mat) + N * iter);
          std::swap_ranges(std::begin(inv) + N * i, std::begin(inv) + N * i + N,
                           std::begin(inv) + N * iter);
        }
        found = true;
        break;
      }
    }
    if (!found)
      return false;
    // float coef = mat[iter + iter * N];
    float coef = mat[iter * (1 + N)];
    for (size_t i = 0; i < N; i++) {
      mat[i + iter * N] /= coef;
      inv[i + iter * N] /= coef;
    }
    for (size_t i = 0; i < N; i++) {
      if (i == iter)
        continue;
      float coef = mat[iter + i * N];
      if (coef != 0.f) {
        for (size_t j = 0; j < N; j++) {
          mat[j + i * N] -= mat[j + iter * N] * coef;
          inv[j + i * N] -= inv[j + iter * N] * coef;
        }
      }
    }
  }
  return true;
}

TEMPLATE_MATRIX_IMPL_HEADER
MatrixO<N> MatrixImpl<N, MatrixO>::invtrans() const {
  MatrixO<N> invtr{};
  bool out = this->inv(invtr);
  if (out) {
    return invtr.transpose();
  } else {
    std::exit(1);
  }
}

TEMPLATE_MATRIX_IMPL_HEADER
float &MatrixImpl<N, MatrixO>::operator[](const size_t idx) {
  return data[idx];
}

TEMPLATE_MATRIX_IMPL_HEADER
const float &MatrixImpl<N, MatrixO>::operator[](const size_t idx) const {
  return data[idx];
}

TEMPLATE_MATRIX_IMPL_HEADER
MatrixO<N> MatrixImpl<N, MatrixO>::operator*(const MatrixO<N> &mat) const {
  MatrixO<N> res{};
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      for (size_t k = 0; k < N; k++) {
        res.data[j + i * N] += data[k + i * N] * mat.data[j + k * N];
      }
    }
  }
  return res;
}

TEMPLATE_MATRIX_IMPL_HEADER
vecnf<N> MatrixImpl<N, MatrixO>::operator*(const vecnf<N> &v) const {
  vecnf<N> res{};
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      res.data[i] += this->data[j + i * N] * v[j];
    }
  }
  return res;
}
