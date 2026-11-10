#include "color.h"
#include "vec3.h"

#include <iostream>
// #include <thread>
// #include <chrono>

int main() {
    // 图片尺寸
    int image_width = 256;
    int image_height = 256;

    // render
    // 图片头文件
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // 写入像素点
    for (int j = 0; j < image_height; j++) {
        // 进度条
        std::clog << "\rScanlines remaining:" << (image_height - j) << ' ' << std::flush;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 慢下来，可以看到Progress Indicator真的在运行，实际运行时删掉
        for(int i = 0; i < image_width; i++) {
            auto pixel_color = color(double(i)/(image_width-1), double(j)/(image_height - 1), 0);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                       \n";
}