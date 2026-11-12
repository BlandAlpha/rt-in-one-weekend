#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
  public:
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const ray&r, interval ray_t, hit_record& rec) const override {
        // 求解方程相交，过程看 https://raytracing.github.io/books/RayTracingInOneWeekend.html 5.1节和6.2节简化
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // 寻找打击点，如果不优于max则舍弃（一般有两个交点）
        auto root = (h - sqrtd) / a;                    // 先找小交点
        if (!ray_t.surrounds(root)) {     // 小交点不在范围里
            root = (h + sqrtd) / a;                     // 再试试大交点
            if (!ray_t.surrounds(root)) { // 大交点也不在？
                return false;                           // 没招了，丢吧
            }
        }
        
        // 更新record
        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

  private:
    point3 center;
    double radius;
};

#endif