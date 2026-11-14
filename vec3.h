#ifndef VEC3_H
#define VEC3_H

class vec3 {
  public:
    double e[3];

    // 构造函数
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    // getter
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // 重载，注意这里把 += -= *= /= 以及 - 定义在了类里面
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator-=(const vec3& v) {
        e[0] -= v.e[0];
        e[1] -= v.e[1];
        e[2] -= v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool near_zero() const {
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    // 生成随机光线
    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

// 为了让代码可读性更好（而不是更类型安全），使用point3别名用于区分
// 实际写作时需要自行区分二者的合法操作，编译器不会报错的
using point3 = vec3;

// 这里把二元运算符定义在了类外面，和上面
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

/// @brief 归一化向量
/// @param v vec3向量
/// @return 归一化后的v
inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while(true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector() {
    while(true) {
        // 在2*2*2的立方体控件随机生成向量
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        // 去掉球外面的向量，用于均匀化生成分布
        if(1e-160 < lensq && lensq <= 1) {
            // 确定没问题了再归一化（计算sqrt）降低开销
            return p / sqrt(lensq);
        }
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)  // 和法线同向
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

/// @brief 计算反射光线
/// @param v vec3 入射光线方向
/// @param n vec3 法线方向
/// @return vec3 反射光线方向
inline vec3 reflect(const vec3& v, const vec3& n) {
    // 参见图15
    vec3 b = -2 * dot(v, n) * n;
    return v + 2*b;
}

/// @brief 计算折射光线
/// @param uv vec3 入射光线方向 (图形学经常这么写)
/// @param n vec3 法线方向
/// @param etai_over_etat double 入射介质ita / 投射介质ita
/// @return vec3 折射光线方向
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    // 斯涅尔定律，参见11.2
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(1.0 - r_out_perp.length_squared()) * n;
    return r_out_parallel + r_out_perp;
}

#endif