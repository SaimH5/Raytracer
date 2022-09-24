#ifndef _SPHERE_h
#define _SPHERE_h

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    sphere(point3 c, double r, color a) : m_center(c), m_radius(r), m_albedo(a) {}

    virtual bool hit(const ray& r, double t_min, double t_max, color& attenuation, hit_record& rec) override 
    {
        auto ray_org_to_center = r.origin() - m_center;
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
        vec3 outward_normal = (rec.p - m_center) / m_radius;
        rec.set_face_normal(r, outward_normal);
        attenuation = m_albedo;
        return true;
    }


private:
    point3 m_center;
    double m_radius;
    color m_albedo;
};

#endif
