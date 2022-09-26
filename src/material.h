#ifndef _MATERIAL_h
#define _MATERIAL_h

#include "ray.h"
#include "hittable.h"

class material
{
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const=0;
};


class lambertian : public material
{
public:
    lambertian(color a) : m_albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const override;

private:
    color m_albedo;
};

bool lambertian::scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const
{
    vec3 scatter_direction = rec.normal + random_unit_vector();
    if(scatter_direction.near_zero()) scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = m_albedo;

    return true;
}


class metal : public material
{
public:
    metal(color a, double fuzz = 0) : m_albedo(a), m_fuzziness(fuzz) { m_fuzziness > 1 ? 1 : m_fuzziness; }

    virtual bool scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const override;

private:
    color m_albedo;
    double m_fuzziness;
};

bool metal::scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const
{
    attenuation = m_albedo;
    vec3 scatter_direction = reflect(rec.normal, r_in.direction());
    scattered = ray(rec.p, scatter_direction + m_fuzziness * random_in_unit_sphere());

    return dot(scattered.direction(), rec.normal) > 0;    
}

#endif
