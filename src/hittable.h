#ifndef _HITTABLE_h
#define _HITTABLE_h

#include "ray.h"
#include "vec3.h"

struct hit_record
{
    double t;
    point3 p;
    vec3 normal;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual bool hit(const ray& r, double t_min, double t_max, color& attenuation, hit_record& rec)=0;
};



#endif
