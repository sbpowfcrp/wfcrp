#pragma once

#include <cmath>

struct Vector2D
{
    double x = 0.0;
    double y = 0.0;

    inline constexpr Vector2D operator+(const Vector2D &other) const { return Vector2D(x + other.x, y + other.y); }
    inline constexpr Vector2D operator-(const Vector2D &other) const { return Vector2D(x - other.x, y - other.y); }
    inline constexpr Vector2D operator*(double t) const { return Vector2D(x * t, y * t); }
    inline constexpr Vector2D operator/(double t) const { return Vector2D(x / t, y / t); }

    inline constexpr Vector2D &operator+=(const Vector2D &other) { x += other.x; y += other.y; return *this; }
    inline constexpr Vector2D &operator-=(const Vector2D &other) { x -= other.x; y -= other.y; return *this; }
    inline constexpr Vector2D &operator*=(double t) { x *= t; y *= t; return *this; }
    inline constexpr Vector2D &operator/=(double t) { x /= t; y /= t; return *this; }

    inline constexpr double Magn() const { return std::sqrt(x * x + y * y); }

    inline constexpr double Cross(const Vector2D &p) const { return x * p.y - y * p.x; }

    inline constexpr double Cross(const Vector2D &a, const Vector2D &b) const { return (a - *this).Cross(b - *this); }
};

inline constexpr Vector2D operator*(double t, Vector2D vector2d) { return vector2d * t; }
