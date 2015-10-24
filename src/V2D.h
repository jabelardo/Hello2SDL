//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_V2D_H
#define HELLO2SDL_V2D_H

#include <math.h>

struct V2D {
  float x;
  float y;
};

float
lengthSquare(const V2D& v2D) {
  return v2D.x * v2D.x + v2D.y * v2D.y;
}

float
length(const V2D& v2D) {
  return sqrtf(lengthSquare(v2D));
}

V2D
operator*(float scalar, const V2D& v2D) {
  return V2D{v2D.x * scalar, v2D.y * scalar};
}

V2D
operator*(const V2D& v2D, float scalar) {
  return V2D{v2D.x * scalar, v2D.y * scalar};
}

V2D &
operator*=(V2D& v2D, float scalar) {
  v2D = v2D * scalar;
  return v2D;
}

void
normalize(V2D& v2D) {
  float l = length(v2D);
  // we never want to attempt to divide by 0
  if (l > 0) {
    v2D *= 1 / l;
  }
}

V2D
operator+(const V2D &v2a, const V2D &v2b) {
  return V2D{v2a.x + v2b.x, v2a.y + v2b.y};
}

V2D &
operator+=(V2D &v2a, const V2D &v2b) {
  v2a = v2a + v2b;
  return v2a;
}

V2D
operator-(const V2D &v2a, const V2D &v2b) {
  return V2D{v2a.x - v2b.x, v2a.y - v2b.y};
}

V2D &
operator-=(V2D &v2a, const V2D &v2b) {
  v2a = v2a - v2b;
  return v2a;
}

V2D
operator/(const V2D &v2, float scalar) {
  return V2D{v2.x / scalar, v2.y / scalar};
}

V2D &
operator/=(V2D &v2, float scalar) {
  v2 = v2 / scalar;
  return v2;
}

#endif //HELLO2SDL_V2D_H
