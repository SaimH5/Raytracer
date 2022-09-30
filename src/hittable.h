#ifndef _HITTABLE_h
#define _HITTABLE_h

#include <memory>
#include "utilities.h"
#include "aabb.h"

// Struct to hold details of individual ray-object intersections
struct hit_record
{
    point3 p;
    vec3 normal;
    double t;
    double u;
    double v;
    bool front_face;
    std::shared_ptr<class material> mat_ptr;

    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract base class for objects that can be hit/intersected by a ray
class hittable
{
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const=0;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const=0;
};



#endif
