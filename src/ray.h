#ifndef _RAY_h
#define _RAY_h

#include "vec3.h"

class ray
{
public:
    ray()=default;
    ray(point3 org, vec3 dir) : m_origin(org), m_direction(dir) {}

    point3 origin() const { return m_origin; }
    vec3 direction() const { return m_direction; }
    vec3 at(const double param) const { return m_origin + param * m_direction; }

private:
    point3 m_origin;
    vec3 m_direction;
};

#endif
