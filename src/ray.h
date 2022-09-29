#ifndef _RAY_h
#define _RAY_h

#include "vec3.h"

// Ray class that starts at an origin and has a direction
class ray
{
public:
    ray()=default;
    ray(point3 org, vec3 dir, double time=0.0) : m_origin(org), m_direction(dir), tm(time) {}

    point3 origin() const { return m_origin; }
    vec3 direction() const { return m_direction; }
    double time() const { return tm; }

    vec3 at(const double param) const { return m_origin + param * m_direction; }

private:
    point3 m_origin;
    vec3 m_direction;
    double tm;
};

#endif
