#pragma once

typedef struct vec2 {
    float x, y;

    vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    vec2 operator +(const vec2& other) const;
    vec2 operator -(const vec2& other) const;
    vec2 operator *(float scalar) const;
    vec2 operator /(float scalar) const;
    vec2 operator *(const vec2& other) const;
    vec2 operator -() const;
    vec2 operator +=(const vec2& other);
    vec2 operator -=(const vec2& other);
    vec2 operator *=(float scalar);
    vec2 operator /=(float scalar);

    float length() const;
    vec2 normalized() const;
} vec2;
