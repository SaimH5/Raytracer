#ifndef _TEXTURE_h
#define _TEXTURE_h

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utilities.h"
#include "perlin.h"

// Base texture class
class texture
{
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};


// Solid color texture 
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


class noise_texture : public texture
{
public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    virtual color value(double u, double v, const point3& p) const override
    {
        return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }

private:
    perlin noise;
    double scale;
};


class image_texture : public texture
{
public:
    const static int bytes_per_pixel = 3;

    image_texture() : m_img(nullptr), m_width(0), m_height(0), m_bytes_per_scanline(0) {}

    image_texture(const char* filename)
    {
        auto components_per_pixel = bytes_per_pixel;

        m_img = stbi_load(filename, &m_width, &m_height, &components_per_pixel, components_per_pixel);

        if(!m_img)
        {
            std::string error = "ERROR: Could not load " + static_cast<std::string>(filename) + ".\n";
            std::cerr << error;
            m_width = m_height = 0;
        }

        m_bytes_per_scanline = bytes_per_pixel * m_width;
    }

    ~image_texture()
    {
        delete m_img;
    }

    virtual color value(double u, double v, const point3& p) const override
    {
        if(m_img == nullptr)
        {   
            return color(0, 1, 1);
        }

        u = clamp(u, 0.0, 1.0);
        // v = clamp(v, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);

        auto i = static_cast<int>(u * m_width);
        auto j = static_cast<int>(v * m_height);

        if(i >= m_width) i = m_width-1;
        if(j >= m_height) j = m_height-1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = m_img + j*m_bytes_per_scanline + i * bytes_per_pixel;

        return color(color_scale*pixel[0],
                     color_scale*pixel[1],
                     color_scale*pixel[2]);
    }

private:
    unsigned char * m_img;
    int m_width, m_height;
    int m_bytes_per_scanline;
};
#endif
