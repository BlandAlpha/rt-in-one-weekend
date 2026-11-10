#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
    public:
        point3 p;
        vec3 normal;
        double t;
};

class hittable {
    public:
        virtual ~hittable() = default;
        // 擂台机制，由物体自行判断是否更优，并返回是或否
        virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif