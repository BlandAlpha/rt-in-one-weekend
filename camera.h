#ifndef CAMERA
#define CAMERA

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 1.0;      // 宽高比
    int    image_width = 400;       // 横向像素数量
    int    samples_per_pixel = 10;  // 每像素随机采样数
    int    max_depth = 10;          // 最大光线弹射次数
    
    /////////// 渲染 ///////////
    void render(const hittable& world) {
        // 初始化
        initialize();

        // 图片头文件
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        // 写入像素点
        for (int j = 0; j < image_height; j++) {
            // 进度条
            std::clog << "\rScanlines remaining:" << (image_height - j) << ' ' << std::flush;
            // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 慢下来，可以看到Progress Indicator真的在运行，实际运行时删掉
            for(int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    // 随机采样光线
                    ray r = get_ray(i, j);
                    // 将颜色信息求和
                    pixel_color += ray_color(r, max_depth, world);
                }
                // 将颜色信息求平均值
                write_color(std::cout, pixel_sample_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                       \n";
    }

  private:
    int    image_height;
    double pixel_sample_scale;
    point3 camera_center;
    point3 pixel00_loc;
    vec3   pixel_delta_u;
    vec3   pixel_delta_v;

    /////////// 初始化 ///////////
    void initialize() {
        // 计算高，并且使其不小于1
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_sample_scale = 1.0 / samples_per_pixel;

        // 相机原点
        camera_center = point3(0, 0, 0);   // 首先使用point3定义构造函数，然后使用auto自动推断类型
        
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
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v/ image_height;

        // viewport左上角，左上角像素采样点
        auto viewport_upper_left = camera_center 
                                - vec3(0, 0, focal_length)     // 世界空间里右x为正，上y为正，右手定则发现朝向我们z为正，因此相机正对-z方向，用减法
                                - viewport_u / 2               // 向上走，对于viewport方向向量来说是反着走，用减法
                                - viewport_v / 2;              // 向左走为负，用减法

        // 左上角像素采样点位置
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    /////////// 随机光线 ///////////
    
    /// @brief 将指向像素 `(i, j)` 的光线在一个像素点范围内进行随机offset
    /// @param i `int`: x轴像素位置
    /// @param j `int`: y轴像素位置
    /// @return `ray`: 新的随机光线
    ray get_ray (int i, int j) {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    /// @brief 返回一个 x=[-0.5, 0.5], y=[-0.5, 0.5] 的采样空间
    /// @return 从(0, 0)指向目标点的 `vec3` 向量
    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    /////////// 计算颜色 ///////////
    color ray_color(const ray& r, int depth, const hittable& world) const {
        // 如果超过最大弹射次数还没找到光源，停止查找光线
        if (depth <= 0)
            return color(0,0,0);

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0,0,0);
        }
        
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 *(unit_direction.y() + 1.0);
        // 线性插值渐变，从蓝色混合到白色，数值越小（越在上面）越蓝
        return (1.0-a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif /* CAMERA */
