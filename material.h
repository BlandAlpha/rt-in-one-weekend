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
1. 光线进入，向四面八方随机弹开，但在弹开的时候把光线的能量乘以R，也就是反照率。
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

#endif