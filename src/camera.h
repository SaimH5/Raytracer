#ifndef _CAMERA_h
#define _CAMERA_h

#include "vec3.h"
#include "ray.h"


class camera
{
public:
    camera(double ar, point3 org) : m_aspect_ratio(ar), m_origin(org) 
    {
        auto viewport_height = 2.0;
        auto viewport_width = m_aspect_ratio * viewport_height;
        auto focal_length = 1.0;

        m_horizontal = vec3(viewport_width, 0, 0);
        m_vertical = vec3(0, viewport_height, 0);
        m_lower_left_corner = m_origin - m_horizontal / 2 - m_vertical / 2 - vec3(0, 0, focal_length);

    }

    ray get_ray(double u, double v)
    {
        return ray(m_origin, m_lower_left_corner + u * m_horizontal + v * m_vertical - m_origin);
    }

private:
     double m_aspect_ratio;
     point3 m_origin;
     vec3 m_horizontal;
     vec3 m_vertical;
     point3 m_lower_left_corner;
};

#endif
