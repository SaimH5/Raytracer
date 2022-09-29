#ifndef _CAMERA_h
#define _CAMERA_h

#include "utilities.h"

// Camera class for handling the viewport and positioning of camera as well as visual effects like Depth of Field
class camera
{
public:
    camera( point3 look_from,
            point3 look_at,
            vec3 vup,
            double vfov,
            double ar,
            double aperture, 
            double focus_dist,
            double tm0 = 0.0,
            double tm1 = 0.0
            ) : m_origin(look_from), time0(tm0), time1(tm1)
    {
        // Adjusts viewport parameters based on supplied field of view value
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = ar * viewport_height;

        // auto focal_length = 1.0;

        // Orthogonal vectors that define the camera's and orientation
        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        m_horizontal = focus_dist * viewport_width * u;
        m_vertical = focus_dist * viewport_height * v;
        m_lower_left_corner = m_origin - m_horizontal / 2 - m_vertical / 2 - focus_dist * w;

        m_lens_radius = aperture / 2;
    }

    ray get_ray(double s, double t)
    {
        // For Depth of Field effect
        vec3 rd = m_lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();

        return ray( m_origin + offset, 
                    m_lower_left_corner + s * m_horizontal + t * m_vertical   - m_origin - offset,
                    random_double(time0, time1));
    }

private:
     point3 m_origin;
     vec3 m_horizontal;
     vec3 m_vertical;
     point3 m_lower_left_corner;
     vec3 u, v, w;
     double m_lens_radius;
     double time0, time1;
};

#endif
