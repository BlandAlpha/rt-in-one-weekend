#ifndef HITTABLE_H
#define HITTABLE_H


class hit_record {
    public:
        point3 p;
        vec3 normal;
        double t;
        bool front_face;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // 光线和法线不同向，则击中物体正面（法线面）
            front_face = dot(r.direction(), outward_normal) < 0;
            // 储存local normal
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;
        // 擂台机制，由物体自行判断是否更优，并返回是或否
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif