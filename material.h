#ifndef MATERIAL
#define MATERIAL

/*
如何定义一个“材质”？不同的材质，光线和表面的交互是不同的。
相同的光线打到了不同的物体上，对路径和能量产生了影响。
所以我们的材质需要做两件事：
1. 生成一个散射光线
2. 说明散射光线相对于入射光线的衰减值
*/

#include "hittable.h"

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in,        // 入射光线
        const hit_record& rec,  // 碰撞点信息
        color& attenuation,     // 输出参数：光线衰减了多少（颜色信息）
        ray& scattered          // 输出参数：散射后的新光线
    ) const {
        return false;
    }
};

/*
朗伯反射其实就是理想的漫反射。
有两种求解思路：
1. 光线进入，向四面八方随机弹开，但在弹开的时候把光线(albedo)的能量乘以R，也就是反照率。
2. 光线进入，根据反照率，有R的概率让光线完整弹开，但也意味着有(1-R)的概率被吸收成为纯黑色
这里我们使用方法1。
*/

class lambertian : public material {
  public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(
        const ray& r_in,
        const hit_record& rec,
        color& attenuation,
        ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // 拦截过小甚至零向量防止NaN
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

class metal : public material {
  public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz) {}

    bool scatter(
        const ray& r_in,
        const hit_record& rec,
        color& attenuation,
        ray& scattered
    ) const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
  
  private:
    color albedo;
    double fuzz;
};

/*
斯涅尔定律的折射
*/
class dielectric : public material {
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(
        const ray& r_in,
        const hit_record& rec,
        color& attenuation,
        ray& scattered
    ) const override {
        attenuation = color(1.0, 1.0, 1.0);
        // 相对折射率，如果光线从内部射入，相对折射率取倒数
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_dir = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_reflect = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_reflect)
            direction = reflect(unit_dir, rec.normal);
        else
            direction = refract(unit_dir, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
    }

  private:
    // 注意，这里填写材质相对于真空/空气的RI，或者材质对于其包裹的材质的相对RI
    // RI_out / RI_in
    double refraction_index;
};

#endif /* MATERIAL */
