#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
    }
    
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    // 线性插值渐变，从蓝色混合到白色，数值越小（越在上面）越蓝
    return (1.0-a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
    ////////// 图片尺寸 //////////

    // 定义图片宽高比以及宽度
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 720;

    // 计算高，并且使其不小于1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    ////////// 世界 //////////

    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));        // 屏幕中间的球
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));   // 巨大的球当地面
    
    ////////// 相机 //////////
    
    // 相机原点
    auto camera_center = point3(0, 0, 0);   // 首先使用point3定义构造函数，然后使用auto自动推断类型
    
    // 摄像机原点到viewport的距离，目前只是用来确定fov
    // viewport是真实存在于世界空间的一个辅助平面，用于确定ray方向
    auto focal_length = 1.0;

    // 真实的宽高比可能因为取整或者最小的1高度产生变化，因此重新计算真实宽高比
    double real_aspect_ratio = double(image_width) / image_height;
    
    // 人眼和相机的视野，通常由“垂直视场角”定义。 因此这里用height来定基准
    auto viewport_height = 2.0;  // 选择2.0是因为好算，上半部分[0, 1]，下半部分[-1, 0]
    auto viewport_width = viewport_height * real_aspect_ratio;

    // viewport方向向量（右为正，下为正）
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // 横纵delta
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v/ image_height;

    // viewport左上角，左上角像素采样点
    auto viewport_upper_left = camera_center 
                             - vec3(0, 0, focal_length)     // 世界空间里右x为正，上y为正，右手定则发现朝向我们z为正，因此相机正对-z方向，用减法
                             - viewport_u / 2               // 向上走，对于viewport方向向量来说是反着走，用减法
                             - viewport_v / 2;              // 向左走为负，用减法

    // 左上角像素采样点位置
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    
    ////////// 渲染 //////////
    
    // 图片头文件
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // 写入像素点
    for (int j = 0; j < image_height; j++) {
        // 进度条
        std::clog << "\rScanlines remaining:" << (image_height - j) << ' ' << std::flush;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 慢下来，可以看到Progress Indicator真的在运行，实际运行时删掉
        for(int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                       \n";
}