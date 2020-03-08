#include "Matrix.h"

vec4f mat4f::operator*(const vec3f &v) const {
  return (*this) * vec4f{v.x, v.y, v.z, 1.f};
}

mat4f mat4f::translate(const vec3f &v) {
  return {1.f, 0.f, 0.f, v.x, 0.f, 1.f, 0.f, v.y,
          0.f, 0.f, 1.f, v.z, 0.f, 0.f, 0.f, 1.f};
}

mat4f mat4f::scale(const vec3f &v) {
  return {v.x, 0.f, 0.f, 0.f, 0.f, v.y, 0.f, 0.f,
          0.f, 0.f, v.z, 0.f, 0.f, 0.f, 0.f, 1.f};
}

mat4f mat4f::rotate(const vec3f &v) {
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

mat4f mat4f::camera(float z) {
  const float dist = 1.3;
  return {1.f / dist, 0.f, 0.f, 0.f, 0.f, 1.f / dist, 0.f,      0.f,
          0.f,        0.f, 1.f, 0.f, 0.f, 0.f,        -1.f / z, 1.f};
}

mat4f mat4f::lookat(const vec3f &center, const vec3f &eye, const vec3f &up) {
  const vec3f camera = eye - center;
  const vec3f zz = camera.normal();
  const vec3f xx = up.cross(zz).normal();
  const vec3f yy = zz.cross(xx).normal();
  return {xx.x, xx.y, xx.z, 0.f, yy.x, yy.y, yy.z, 0.f,
          zz.x, zz.y, zz.z, 0.f, 0.f,  0.f,  0.f,  1.f};
}

mat4f mat4f::view(const vec3f &center, const vec3f &eye, const vec3f &up) {
  return lookat(center, eye, up) * mat4f::translate(-center);
}