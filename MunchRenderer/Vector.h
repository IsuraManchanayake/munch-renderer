#pragma once

#include <array>
#include <cmath>
#include <type_traits>
#include <utility>

template <size_t DIM, typename T> struct VecBase;

template <typename T> struct VecBase<2, T> {
  union {
    struct {
      T x;
      T y;
    };
    std::array<T, 2> data;
  };
  VecBase() : x(), y() {}
  VecBase(T x, T y) : x(x), y(y) {}
};

template <typename T> struct VecBase<3, T> {
  union {
    struct {
      T x;
      T y;
      T z;
    };
    std::array<T, 3> data;
  };
  VecBase() : x(), y(), z() {}
  VecBase(T x, T y, T z) : x(x), y(y), z(z) {}
};

template <typename T> struct VecBase<4, T> {
  union {
    struct {
      T x;
      T y;
      T z;
      T w;
    };
    std::array<T, 4> data;
  };
  VecBase() : x(), y(), z(), w() {}
  VecBase(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
};

template <size_t DIM, typename T> struct Vector : VecBase<DIM, T> {
  static constexpr size_t dim = DIM;
  using type = T;
  static Vector<DIM, T> zero;

  template <typename... U> Vector(U &&... args);
  template <size_t DIM1> Vector<DIM1, T> as() const;

  T &operator[](size_t idx);
  const T &operator[](size_t idx) const;
  Vector<DIM, T> operator+(const Vector<DIM, T> &rhs) const;
  Vector<DIM, T> operator-(const Vector<DIM, T> &rhs) const;
  Vector<DIM, T> operator-() const;
  //template <
  //    typename = typename std::enable_if<!std::is_same<float, T>::value>::type>
  //Vector<DIM, float> operator*(const float &rhs) const;
  //template <
  //    typename = typename std::enable_if<!std::is_same<float, T>::value>::type>
  //Vector<DIM, float> operator/(const float &rhs) const;
  Vector<DIM, T> operator*(const T &rhs) const;
  Vector<DIM, T> operator/(const T &rhs) const;
  //template <
  //    size_t DIM1, typename T1,
  //    typename = typename std::enable_if<!std::is_same<float, T1>::value>::type>
  //friend Vector<DIM1, T1> operator*(const float &lhs,
  //                                  const Vector<DIM1, T1> &rhs);
  template <size_t DIM1, typename T1>
  friend Vector<DIM1, T1> operator*(const T1 &lhs, const Vector<DIM1, T1> &rhs);
  T dot(const Vector<DIM, T> &rhs) const;
  Vector<DIM, T>
  mul(const Vector<DIM, T> &rhs) const; // element-wise multiplication
  Vector<DIM, T> cross(const Vector<DIM, T> &rhs) const;

  T dist(const Vector<DIM, T> &rhs) const;
  T norm() const;
  Vector<DIM, T> normal() const;
  template <typename T1>
  Vector<DIM, T> bary(const Vector<DIM, T1> &v0, const Vector<DIM, T1> &v1,
                      const Vector<DIM, T1> &v2) const;

  Vector<DIM, T> translate(const Vector<DIM, T> &v) const;
  Vector<DIM, T> scale(const Vector<DIM, T> &v) const;
  Vector<DIM, T> rotx(float a) const;
  Vector<DIM, T> roty(float a) const;
  Vector<DIM, T> rotz(float a) const;
};

using vec2i = Vector<2, int>;
using vec3i = Vector<3, int>;
using vec2f = Vector<2, float>;
using vec3f = Vector<3, float>;
using vec4f = Vector<4, float>;

struct Matrix {
  std::array<float, 16> data;

  Matrix();
  template <typename... T> Matrix(T &&... init);

  Matrix transpose() const;
  bool inv(Matrix &invmat) const;
  Matrix invtrans() const;

  float &operator[](const size_t idx);
  const float &operator[](const size_t idx) const;

  Matrix operator*(const Matrix &mat) const;
  friend vec4f operator*(const vec4f &v, const Matrix &mat);
  friend vec4f operator*(const vec3f &v, const Matrix &mat);

  static Matrix translate(const vec3f &v);
  static Matrix scale(const vec3f &v);
  static Matrix rotate(const vec3f &v);
};

//using mat4f = Matrix;

#define TEMPLATE_HEADER template <size_t DIM, typename T>

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::zero{0, 0, 0};

TEMPLATE_HEADER
template <typename... U>
Vector<DIM, T>::Vector(U &&... args)
    : VecBase<DIM, T>(std::forward<U>(args)...) {}

TEMPLATE_HEADER
template <size_t DIM1> Vector<DIM1, T> Vector<DIM, T>::as() const {
  Vector<DIM1, T> v{};
  for (size_t i = 0, s = DIM > DIM1 ? DIM1 : DIM; i < s; i++) {
    v.data[i] = this->data[i];
  }
  return v;
}

TEMPLATE_HEADER
T &Vector<DIM, T>::operator[](size_t idx) { return this->data[idx]; }

TEMPLATE_HEADER
const T &Vector<DIM, T>::operator[](size_t idx) const {
  return this->data[idx];
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::operator+(const Vector<DIM, T> &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] += rhs[i];
  }
  return result;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::operator-(const Vector<DIM, T> &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] -= rhs[i];
  }
  return result;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::operator-() const {
  Vector<DIM, T> result;
  for (size_t i = 0; i < DIM; i++) {
    result[i] = -this->data[i];
  }
  return result;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::operator*(const T &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] *= rhs;
  }
  return result;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::operator/(const T &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] /= rhs;
  }
  return result;
}

template <size_t DIM1, typename T1>
Vector<DIM1, T1> operator*(const T1 &lhs, const Vector<DIM1, T1> &rhs) {
  return rhs * lhs;
}

TEMPLATE_HEADER
T Vector<DIM, T>::dot(const Vector<DIM, T> &rhs) const {
  T res{};
  for (size_t i = 0; i < DIM; i++) {
    res += this->data[i] * rhs.data[i];
  }
  return res;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::mul(const Vector<DIM, T> &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] *= rhs[i];
  }
  return result;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::cross(const Vector<DIM, T> &rhs) const {
  const Vector<DIM, T> &lhs = *this;
  return {lhs.y * rhs.z - rhs.y * lhs.z, -lhs.x * rhs.z + rhs.x * lhs.z,
          lhs.x * rhs.y - rhs.x * lhs.y};
}

TEMPLATE_HEADER
T Vector<DIM, T>::dist(const Vector<DIM, T> &rhs) const {
  T result();
  return (*this - rhs).norm();
}

TEMPLATE_HEADER
T Vector<DIM, T>::norm() const { return sqrt(this->dot(*this)); }

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::normal() const { return (*this) / norm(); }

TEMPLATE_HEADER
template <typename T1>
Vector<DIM, T> Vector<DIM, T>::bary(const Vector<DIM, T1> &v0,
                                    const Vector<DIM, T1> &v1,
                                    const Vector<DIM, T1> &v2) const {
  Vector<DIM, T> xx{v0.x - v2.x, v1.x - v2.x, v2.x - this->x};
  Vector<DIM, T> yy{v0.y - v2.y, v1.y - v2.y, v2.y - this->y};
  Vector<DIM, T> ortho(xx.cross(yy));
  ortho = ortho / ortho.z;
  return {ortho.x, ortho.y, 1 - ortho.x - ortho.y};
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::translate(const Vector<DIM, T> &v) const {
  return *this + v;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::scale(const Vector<DIM, T> &v) const {
  return this->mul(v);
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::rotx(float a) const {
  return {this->x, this->y * cos(a) + this->z * sin(a),
          -this->y * sin(a) + this->z * cos(a)};
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::roty(float a) const {
  return {this->x * cos(a) - this->z * sin(a), this->y,
          this->x * sin(a) + this->z * cos(a)};
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::rotz(float a) const {
  return {this->x * cos(a) + this->y * sin(a),
          -this->x * sin(a) + this->y * cos(a), this->z};
}
