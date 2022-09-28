#ifndef _HITTABLE_LIST_h
#define _HITTABLE_LIST_h

#include <vector>
#include <memory>
#include "hittable.h"

// Derived hittable class that holds a list of hittable objects in an std::vector and 
// for a given ray, figures out which, if any, of the stored objects are hit and which one is closest
class hittable_list : public hittable
{
public:
    hittable_list()=default;
    hittable_list(std::shared_ptr<hittable> h) { objects.push_back(h); }

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    void clear() { objects.clear(); }
    void add(std::shared_ptr<hittable> h) { objects.push_back(h); }
private:
    std::vector<std::shared_ptr<hittable>> objects;
};

    bool hittable_list::hit(const ray&r, double t_min, double t_max, hit_record& rec) const
    {
        bool hit_anything = false;
        double closest_so_far = t_max;
        hit_record temp_rec;
        for(const auto& obj : objects)
        {
            if(obj->hit(r, t_min, closest_so_far, temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }



#endif
