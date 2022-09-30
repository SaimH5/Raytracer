#ifndef _MOVING_SPHERE_h
#define _MOVING_SPHERE_h

#include "hittable.h"
#include "vec3.h"

class material;

// Class for moving sphere primitives that can be intersected by a ray and rendered
class moving_sphere : public hittable
{
public:
    moving_sphere(point3 c0, point3 c1, double t0, double t1, double r, std::shared_ptr<material> mat) 
    : m_center0(c0), m_center1(c1), time0(t0), time1(t1), m_radius(r), m_mat(mat) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override 
    {
        auto ray_org_to_center = r.origin() - center(r.time());
        auto a = r.direction().length_squared();
        auto b = 2 * dot(r.direction(), ray_org_to_center);
        auto c = ray_org_to_center.length_squared() - m_radius * m_radius;

        auto discriminant = b * b - 4 * a * c;
        if(discriminant < 0) return false;

        auto sqrt_disc = std::sqrt(discriminant);
        auto root = (-b - sqrt_disc) / (2*a);
        if(root < t_min || root > t_max) 
        {
            root = (-b + sqrt_disc) / (2*a);
            if(root < t_min || root > t_max) 
            {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center(r.time())) / m_radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = m_mat;
        return true;
    }

    virtual bool bounding_box(double tm0, double tm1, aabb& output_box) const override
    {
        aabb box0(center(tm0) - vec3(m_radius), center(tm0) + vec3(m_radius));
        aabb box1(center(tm1) - vec3(m_radius), center(tm1) + vec3(m_radius));

        output_box = surrounding_box(box0, box1);
        return true;
    }

    point3 center(double time) const
    {
        return m_center0 + ((time - time0) / (time1 - time0)) * (m_center1 - m_center0);
    }

private:
    point3 m_center0, m_center1;
    double time0, time1;
    double m_radius;
    std::shared_ptr<material> m_mat;
};

#endif
