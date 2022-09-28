#ifndef _MATERIAL_h
#define _MATERIAL_h

#include "ray.h"
#include "hittable.h"

// Base material class with virtual scatte functions for non-emmisive surfaces and emitted function for emissive surfaces
class material
{
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const=0;
    virtual color emitted() const
    {
        return color(0, 0, 0);
    }
};

// Derived material class for diffuse/lambertian surfaces
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

// Derived material class for metal/reflective surfaces
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

// Derived material class for dielectric/transparent surfaces (e.g. glass)
class dielectric : public material
{
public:
    dielectric(double ir = 1) : m_refractive_index(ir) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const override;

private:
    double m_refractive_index;

    static double reflectance(double cosine, double ref_idx)
    {
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 *= r0;
        return r0 + (1-r0)*pow(1 - cosine, 5);
    }
};

bool dielectric::scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const
{
    attenuation = color(1, 1, 1);
    double refraction_ratio = rec.front_face ? (1.0 / m_refractive_index) : m_refractive_index;
    
    auto unit_v = unit_vector(r_in.direction());
    auto cos_theta = dot(-unit_v, rec.normal);
    auto sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;

    vec3 direction;
    if(cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
    {
        direction = reflect(rec.normal, r_in.direction());
    }
    else
    {
        direction = refract(rec.normal, r_in.direction(), refraction_ratio);
    }

    scattered = ray(rec.p, direction);
    return true;    
}

// Derived material class for emissive surfaces
class diffuse_light : public material
{
public:
    diffuse_light(color a) : m_albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, ray& scattered, color& attenuation) const override
    {
        return false;
    }

    virtual color emitted() const override { return m_albedo; }

private:
    color m_albedo;
};
#endif
