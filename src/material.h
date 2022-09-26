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
    scattered = ray(rec.p, rec.normal + random_unit_vector());
    attenuation = m_albedo;

    return true;
}

#endif
