#ifndef _VEC3_h
#define _VEC3_h

#include <array>
#include <cmath>
#include <iostream>

class vec3
{
public:
    // Constructors
    vec3(double x, double y, double z) 
    {
        e[0] = x;
        e[1] = y;
        e[2] = z;
    }

    vec3(const vec3& other)=default;
    vec3& operator=(const vec3& other)=default;

    vec3(const vec3&& other) : e(std::move(other.e)) {}

    // Access individual elements
    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double z() const { return e[2]; }

    // Indexing operators
    inline double operator[](int idx) const { return e[idx]; }
    inline double& operator[](int idx) { return e[idx]; }

    // Arithmetic operators
    inline vec3& operator+=(const vec3& other)
    {
        e[0] += other[0];
        e[1] += other[1];
        e[2] += other[2];
        return *this;
    }

    inline vec3& operator-=(const vec3& other)
    {
        e[0] -= other[0];
        e[1] -= other[1];
        e[2] -= other[2];
        return *this;
    }

    inline vec3& operator*=(const vec3& other)
    {
        e[0] *= other[0];
        e[1] *= other[1];
        e[2] *= other[2];
        return *this;
    }

    inline vec3& operator*=(const double k)
    {
        e[0] *= k;
        e[1] *= k;
        e[2] *= k;
        return *this;
    }

    inline vec3& operator/=(const vec3& other)
    {
        e[0] /= other[0];
        e[1] /= other[1];
        e[2] /= other[2];
        return *this;
    }

    inline vec3& operator/=(const double k)
    {
        e[0] /= k;
        e[1] /= k;
        e[2] /= k;
        return *this;
    }


    inline double length_squared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline double length() const
    {
        return std::sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }

private:
    std::array<double, 3> e;
};

// Using's

using point3 = vec3;
using color = vec3;

// Operator overloads

inline vec3 operator+(const vec3& v1, const vec3& v2)
{
    return vec3(v1[0] + v2[0],
                v1[1] + v2[1],
                v1[2] + v2[2]);
}

inline vec3 operator+(const vec3& v, const double k)
{
    return vec3(v[0] + k,
                v[1] + k,
                v[2] + k);
}

inline vec3 operator+(const double k, const vec3& v)
{
    return vec3(k + v[0],
                k + v[1],
                k + v[2]);
}

inline vec3 operator-(const vec3& v1, const vec3& v2)
{
    return vec3(v1[0] - v2[0],
                v1[1] - v2[1],
                v1[2] - v2[2]);
}

inline vec3 operator-(const vec3& v, const double k)
{
    return vec3(v[0] - k,
                v[1] - k,
                v[2] - k);
}

inline vec3 operator-(const double k, const vec3& v)
{
    return vec3(k - v[0],
                k - v[1],
                k - v[2]);
}

inline vec3 operator*(const vec3& v1, const vec3& v2)
{
    return vec3(v1[0] * v2[0],
                v1[1] * v2[1],
                v1[2] * v2[2]);
}

inline vec3 operator*(const vec3& v, const double k)
{
    return vec3(v[0] * k,
                v[1] * k,
                v[2] * k);
}

inline vec3 operator*(const double k, const vec3& v)
{
    return vec3(k * v[0],
                k * v[1],
                k * v[2]);
}

inline vec3 operator/(const vec3& v1, const vec3& v2)
{
    return vec3(v1[0] / v2[0],
                v1[1] / v2[1],
                v1[2] / v2[2]);
}

inline vec3 operator/(const vec3& v1, const double k)
{
    return vec3(v1[0] / k,
                v1[1] / k,
                v1[2] / k);
}

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
    out << v[0] << " " << v[1] << " " << v[2] << "\n";
    return out;
}

// Vector operations

inline double dot(const vec3& v1, const vec3& v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline vec3 cross(const vec3& v1, const vec3& v2)
{
    return vec3( (v1[1] * v2[2] - v1[2] * v2[1]),
                -(v1[0] * v2[2] - v1[2] * v2[0]),
                 (v1[0] * v2[1] - v1[1] * v2[0]));
}

inline vec3 unit_vector(const vec3& v)
{
    return v / v.length();
}

#endif
