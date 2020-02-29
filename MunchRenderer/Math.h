#pragma once

template <typename T> inline T clamp(T x, T min, T max) {
  return x < min ? min : (x > max ? max : x);
}

inline float map(float x, float a0, float b0, float a1, float b1) {
  return a1 + (b1 - a1) * (x - a0) / (b0 - a0);
}

inline float lerp(float x0, float x1, float f) { return x0 + f * (x1 - x0); }

inline float norm(float x, float x0, float x1) { return (x - x0) / (x1 - x0); }