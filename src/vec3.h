#ifndef _VEC3_h
#define _VEC3_h

// #include <array>
#include <iostream>

#include "utilities.h"

// Vector class for storing 3D points, vectors, and colors
// plus implements basic vector operations
class vec3
{
public:
    // Constructors
    vec3() : e{0, 0, 0} {}
    vec3(double x, double y, double z) : e{x, y, z} {}
    vec3(double x) : e{x, x, x} {}

    vec3(vec3& other)=default;
    vec3(const vec3& other)=default;
    vec3& operator=(const vec3& other)=default;

    // vec3(const vec3&& other) : e(std::move(other.e)) {}

    // Access individual elements
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // Indexing operators
    inline double operator[](int idx) const { return e[idx]; }
    inline double& operator[](int idx) { return e[idx]; }


    // Arithmetic operators
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    vec3& operator+=(const vec3& other)
    {
        e[0] += other[0];
        e[1] += other[1];
        e[2] += other[2];
        return *this;
    }

    vec3& operator-=(const vec3& other)
    {
        e[0] -= other[0];
        e[1] -= other[1];
        e[2] -= other[2];
        return *this;
    }

    vec3& operator*=(const vec3& other)
    {
        e[0] *= other[0];
        e[1] *= other[1];
        e[2] *= other[2];
        return *this;
    }

    vec3& operator*=(const double k)
    {
        e[0] *= k;
        e[1] *= k;
        e[2] *= k;
        return *this;
    }

    vec3& operator/=(const vec3& other)
    {
        e[0] /= other[0];
        e[1] /= other[1];
        e[2] /= other[2];
        return *this;
    }

    vec3& operator/=(const double k)
    {
        e[0] /= k;
        e[1] /= k;
        e[2] /= k;
        return *this;
    }

    // Vector convenience operations
    double length_squared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline double length() const
    {
        return std::sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }

    bool near_zero() const
    {
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < 0) && (fabs(e[2]) < 0);
    }

    // Random vector generation functions
    inline static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    inline static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

private:
    // std::array<double, 3> e;
    double e[3];
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



// Random vector generation functions for computing scattering
// of ray upon intersection with surfaces and for visual effects
vec3 random_in_unit_sphere()
{
    while(true)
    {
        vec3 r = vec3::random(-1, 1);
        if(r.length_squared() > 1) continue;
        return r;
    }
}

vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

vec3 random_in_unit_hemisphere(const vec3& normal)
{
    vec3 in_unit_sphere = random_in_unit_sphere();
    if(dot(in_unit_sphere, normal) > 0.0)
    {
        return in_unit_sphere;
    }
    
    return -in_unit_sphere;
}

vec3 random_in_unit_disk()
{
    while(true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if(p.length_squared() >= 1) continue;
        return p;
    }
}



// Vector functions for computing ray reflection and refraction upon hitting materials
vec3 reflect(const vec3& normal, const vec3& v)
{
    return v + 2 * dot(normal, -v) * normal;
}

vec3 refract(const vec3& normal, const vec3& v, double etai_over_etat)
{
    vec3 unit_v = unit_vector(v);
    auto cos_theta = fmin(dot(-unit_v, normal), 1.0);

    vec3 r_out_perp = etai_over_etat * (unit_v + cos_theta * normal);
    vec3 r_out_parallel = sqrt(fabs(1 - r_out_perp.length_squared())) * - normal;
    return r_out_perp + r_out_parallel;
}
#endif
