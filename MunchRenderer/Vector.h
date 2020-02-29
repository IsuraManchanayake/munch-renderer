#pragma once

#include <array>
#include <utility>

template <size_t DIM, typename T> struct VecBase;

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

template <size_t DIM, typename T> struct Vector : VecBase<DIM, T> {
  static constexpr T dim = DIM;
  typedef typename T type;
  static Vector<DIM, T> zero;

  Vector(std::array<T, DIM> data);
  template <typename... U> Vector(U &&... args);

  T &operator[](size_t idx);
  const T &operator[](size_t idx) const;
  Vector<DIM, T> operator+(const Vector<DIM, T> &rhs) const;
  Vector<DIM, T> operator-(const Vector<DIM, T> &rhs) const;
  T operator*(const Vector<DIM, T> &rhs) const; // dot product
  Vector<DIM, T> operator-() const;
  template <typename U> Vector<DIM, T> operator*(const U &rhs) const;
  template <typename U> Vector<DIM, T> operator/(const U &rhs) const;
  template <typename U>
  friend Vector<DIM, T> operator*(const U &lhs, const Vector<DIM, T> &rhs);
  T dot(const Vector<DIM, T> &rhs)
      const; // dot product, same as above Vector::operator*(Vector)
  Vector<DIM, T>
  mul(const Vector<DIM, T> &rhs) const; // element-wise multiplication
  Vector<DIM, T> cross(const Vector<DIM, T> &rhs) const;

  T dist(const Vector<DIM, T> &rhs) const;
  T norm() const;
  Vector<DIM, T> normal() const;
};

#define TEMPLATE_HEADER template <size_t DIM, typename T>

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::zero;

TEMPLATE_HEADER
template <typename... U>
Vector<DIM, T>::Vector(U &&... args)
    : VecBase<DIM, T>(std::forward<U &&>(args)...) {}

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
T Vector<DIM, T>::operator*(const Vector<DIM, T> &rhs) const {
  T result;
  for (size_t i = 0; i < DIM; i++) {
    result += this->data[i] * rhs[i];
  }
  return result;
}

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::operator-() const {
  Vector<DIM, T> result;
  for (size_t i = 0; i < DIM; i++) {
    result[i] = -this->data[i];
  }
  result;
}

TEMPLATE_HEADER
template <typename U>
Vector<DIM, T> Vector<DIM, T>::operator*(const U &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] *= rhs;
  }
  return rhs;
}

TEMPLATE_HEADER
template <typename U>
Vector<DIM, T> Vector<DIM, T>::operator/(const U &rhs) const {
  Vector<DIM, T> result(*this);
  for (size_t i = 0; i < DIM; i++) {
    result[i] /= rhs;
  }
  return rhs;
}

template <size_t DIM, typename T, typename U>
Vector<DIM, T> operator*(const U &lhs, const Vector<DIM, T> &rhs) {
  return rhs * lhs;
}

TEMPLATE_HEADER
T Vector<DIM, T>::dot(const Vector<DIM, T> &rhs) const { return (*this) * rhs; }

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
  return zero;
}

TEMPLATE_HEADER
typename T Vector<DIM, T>::dist(const Vector<DIM, T> &rhs) const {
  T result();
  return (*this - rhs).norm();
}

TEMPLATE_HEADER
T Vector<DIM, T>::norm() const { return (*this) * (*this); }

TEMPLATE_HEADER
Vector<DIM, T> Vector<DIM, T>::normal() const { return (*this) / norm(); }

typedef typename Vector<2, int> vec2i;
typedef typename Vector<3, int> vec3i;
typedef typename Vector<2, float> vec2f;
typedef typename Vector<3, float> vec3f;
