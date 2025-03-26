#include <cmath>

#include "math.hpp"

vec2 vec2::operator +(const vec2& other) const {
    return vec2(x + other.x, y + other.y);
}

vec2 vec2::operator -(const vec2& other) const {
    return vec2(x - other.x, y - other.y);
}

vec2 vec2::operator *(float scalar) const {
    return vec2(x * scalar, y * scalar);
}

vec2 vec2::operator /(float scalar) const {
    return vec2(x / scalar, y / scalar);
}

vec2 vec2::operator *(const vec2& other) const {
    return x * other.x + y * other.y;
}

vec2 vec2::operator -() const {
    return vec2(-x, -y);
}

vec2 vec2::operator +=(const vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

vec2 vec2::operator -=(const vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

vec2 vec2::operator *=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

vec2 vec2::operator /=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

float vec2::length() const {
    return sqrt(x * x + y * y);
}

vec2 vec2::normalized() const {
    return *this / length();
}
