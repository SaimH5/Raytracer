#ifndef _HITTABLE_h
#define _HITTABLE_h

#include "ray.h"
#include "vec3.h"

class hittable
{
public:
    virtual bool hit(const ray& r)=0;
    virtual color hit_color() const=0;
};



#endif
