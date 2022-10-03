#ifndef _AARECT_h
#define _AARECT_h

#include "utilities.h"
#include "hittable.h"

class material;

class xy_rect : public hittable
{
public:
    xy_rect() {}
    xy_rect(double x0_, double x1_, double y0_, double y1_, double k, shared_ptr<material> m_ptr) 
    : x0(x0_), x1(x1_), y0(y0_), y1(y1_), z(k), mat(m_ptr) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(point3(x0, y0, z-0.0001), point3(x1, y1, z+0.0001));
        return true;
    }

    
private:
    double x0, y0, x1, y1, z;
    shared_ptr<material> mat;
};

bool xy_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    auto t = (z - r.origin().z()) / r.direction().z();
    if(t < t_min || t > t_max) return false;

    auto x = r.origin().x() + t*r.direction().x();
    auto y = r.origin().y() + t*r.direction().y();

    if(x < x0 || x > x1 || y < y0 || y > y1) return false;
    // Hit confirmed

    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    auto outward_normal = vec3(0, 0, 1);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat;
    rec.p = r.at(t);
    return true;
}


class xz_rect : public hittable
{
public:
    xz_rect() {}
    xz_rect(double x0_, double x1_, double z0_, double z1_, double y_, shared_ptr<material> mat_) 
    : x0(x0_), x1(x1_), z0(z0_), z1(z1_), y(y_), mat(mat_) {} 

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(point3(x0, y-0.0001, z0), point3(x1, y+0.0001, z1));
        return true;;
    }
private:
    double x0, x1, z0, z1, y;
    shared_ptr<material> mat;
};

bool xz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    auto t = (y - r.origin().y()) / r.direction().y();
    if(t < t_min || t > t_max) return false;

    auto x = r.origin().x() + t * r.direction().x();
    auto z = r.origin().z() + t * r.direction().z();
    if(x < x0 || x > x1 || z < z0 || z > z1) return false;

    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    vec3 outward_normal(0, 1, 0);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat;
    rec.p = r.at(rec.t);
    return true;
}


class yz_rect : public hittable
{
public:
    yz_rect() {}
    yz_rect(double y0_, double y1_, double z0_, double z1_, double x_, shared_ptr<material> mat_)
    : y0(y0_), y1(y1_), z0(z0_), z1(z1_), x(x_), mat(mat_) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        output_box = aabb(point3(x-0.0001, y0, z0), point3(x+0.0001, y1, z1));
        return true;
    }
private:
    double y0, y1, z0, z1, x;
    shared_ptr<material> mat;
};

bool yz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    auto t = (x - r.origin().x()) / r.direction().x();
    if(t < t_min || t > t_max) return false;

    auto y = r.origin().y() + t * r.direction().y();
    auto z = r.origin().z() + t * r.direction().z();
    if(y < y0 || y > y1 || z < z0 || z > z1) return false;

    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    vec3 outward_normal(1, 0, 0);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat;
    rec.p = r.at(t);
    return true;
}

#endif
