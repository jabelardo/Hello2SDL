//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_VECTOR2D_H
#define HELLO2SDL_VECTOR2D_H

#include <math.h>

struct Vector2D {
  float x;
  float y;

  float lengthSquare() {
    return x * x + y * y;
  }

  float length() {
    return sqrtf(lengthSquare());
  }

  void normalize() {
    float l = length();
    // we never want to attempt to divide by 0
    if (l > 0) {
      *this *= 1 / l;
    }
  }

  Vector2D operator+(const Vector2D &v2) const {
    return Vector2D{x + v2.x, y + v2.y};
  }

  Vector2D &operator+=(const Vector2D &v2) {
    *this = *this + v2;
    return *this;
  }

  Vector2D operator*(float scalar) {
    return Vector2D{x * scalar, y * scalar};
  }

  Vector2D& operator*=(float scalar) {
    *this = *this * scalar;
    return *this;
  }

  Vector2D operator-(const Vector2D& v2) const {
    return Vector2D{x - v2.x, y - v2.y};
  }

  Vector2D& operator-=(const Vector2D& v2) {
    *this = *this - v2;
    return *this;
  }

  Vector2D operator/(float scalar) {
    return Vector2D{x / scalar, y / scalar};
  }

  Vector2D& operator/=(float scalar) {
    *this = *this / scalar;
    return *this;
  }
};

#endif //HELLO2SDL_VECTOR2D_H
