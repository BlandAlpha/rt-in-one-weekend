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

#endif /* MATERIAL */
