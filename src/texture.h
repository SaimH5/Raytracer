#ifndef _TEXTURE_h
#define _TEXTURE_h

#include "utilities.h"

class texture
{
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture
{
public:
    solid_color() {}
    solid_color(color c) : m_color_value(c) {}

    solid_color(double red, double green, double blue) : m_color_value(color(red, green, blue)) {}

    virtual color value(double u, double v, const point3& p) const override
    {
        return m_color_value;
    }

private:
    color m_color_value;
};


class checker_texture : public texture
{
public:
    checker_texture() {}

    checker_texture(shared_ptr<texture> od, shared_ptr<texture> ev) : odd(od), even(ev) {}

    checker_texture(color c1, color c2) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    virtual color value(double u, double v, const point3& p) const override
    {
        auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if(sines < 0) 
        {
            return odd->value(u, v, p);
        }
        else
        {
            return even->value(u, v, p);
        }
    } 

private:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};
#endif
