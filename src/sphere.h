#ifndef _SPHERE_h
#define _SPHERE_h

#include "hittable.h"
#include "utilities.h"

// Class for sphere primitives that can be intersected by a ray and rendered
class sphere : public hittable
{
public:
    sphere(point3 c, double r, std::shared_ptr<material> mat) : m_center(c), m_radius(r), m_mat(mat) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override 
    {
        auto ray_org_to_center = r.origin() - m_center;
        auto a = r.direction().length_squared();
        auto half_b = dot(r.direction(), ray_org_to_center);
        auto c = ray_org_to_center.length_squared() - m_radius * m_radius;

        auto discriminant = half_b * half_b - a * c;
        if(discriminant < 0) return false;

        auto sqrt_disc = std::sqrt(discriminant);
        auto root = (-half_b - sqrt_disc) / a;
        if(root < t_min || root > t_max) 
        {
            root = (-half_b + sqrt_disc) / a;
            if(root < t_min || root > t_max) 
            {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - m_center) / m_radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat_ptr = m_mat;
        return true;
    }

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(m_center - vec3(m_radius), m_center + vec3(m_radius));
        return true;
    }

private:
    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }


    point3 m_center;
    double m_radius;
    std::shared_ptr<material> m_mat;
};

#endif
