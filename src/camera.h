#ifndef _CAMERA_h
#define _CAMERA_h

#include "vec3.h"
#include "ray.h"


class camera
{
public:
    camera( point3 look_from,
            point3 look_at,
            vec3 vup,
            double vfov,
            double ar
            ) : m_origin(look_from) 
    {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = ar * viewport_height;

        // auto focal_length = 1.0;

        auto w = unit_vector(look_from - look_at);
        auto u = unit_vector(cross(vup, w));
        auto v = cross(w, u);

        m_horizontal = viewport_width * u;
        m_vertical = viewport_height * v;
        m_lower_left_corner = m_origin - m_horizontal / 2 - m_vertical / 2 - w;

    }

    ray get_ray(double u, double v)
    {
        return ray(m_origin, m_lower_left_corner + u * m_horizontal + v * m_vertical - m_origin);
    }

private:
     point3 m_origin;
     vec3 m_horizontal;
     vec3 m_vertical;
     point3 m_lower_left_corner;
};

#endif
