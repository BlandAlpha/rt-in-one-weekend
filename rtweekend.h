#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// 工具函数

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

/// @brief 生成一个 [0, 1) 的随机 `double` 并返回
/// @return 
inline double random_double() {
    return std::rand() / (RAND_MAX+1.0);
}

/// @brief 生成一个 [min, max) 的随机 `double` 并返回
/// @param min `double` 下界
/// @param max `double` 上界
/// @return 
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif