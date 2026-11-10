#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>

#include "vec3.h"   // 注意：vec3.h 里如果 include 了 rtweekend.h，先确保它能单独编译

// ---------- 测试小工具 ----------
static int g_pass = 0;
static int g_fail = 0;

// 浮点近似比较：绝对误差 + 相对误差都考虑
static bool approx(double a, double b, double eps = 1e-9) {
    double diff = std::fabs(a - b);
    if (diff <= eps) return true;
    double scale = std::max(std::fabs(a), std::fabs(b));
    return diff <= eps * scale;
}

static bool near(const vec3& u, const vec3& v, double eps = 1e-9) {
    return approx(u.e[0], v.e[0], eps) &&
           approx(u.e[1], v.e[1], eps) &&
           approx(u.e[2], v.e[2], eps);
}

// 每条检查打印 PASS/FAIL，并统计
static void check(bool cond, const std::string& what, int line) {
    if (cond) {
        ++g_pass;
        std::cout << "  [PASS] " << what << "\n";
    } else {
        ++g_fail;
        std::cout << "  [FAIL] " << what << "   (line " << line << ")\n";
    }
}

#define CHECK(cond) check((cond), #cond, __LINE__)

// ---------- 测试 ----------
int main() {
    std::cout << "==== vec3.h 功能测试 ====\n";

    // 1. 默认构造：三个分量必须是 0（验证成员初始化列表生效）
    std::cout << "\n[1] 默认构造\n";
    {
        vec3 v;
        CHECK(approx(v.e[0], 0.0));
        CHECK(approx(v.e[1], 0.0));
        CHECK(approx(v.e[2], 0.0));
    }

    // 2. 三参构造 + 访问器
    std::cout << "\n[2] 构造与访问器\n";
    {
        vec3 v(1.0, 2.0, 3.0);
        CHECK(approx(v.x(), 1.0));
        CHECK(approx(v.y(), 2.0));
        CHECK(approx(v.z(), 3.0));
        CHECK(approx(v[0], 1.0));
        CHECK(approx(v[1], 2.0));
        CHECK(approx(v[2], 3.0));
    }

    // 3. 标量乘：两个方向都要支持
    std::cout << "\n[3] 标量乘\n";
    {
        vec3 v(1.0, 2.0, 3.0);
        CHECK(near(2.0 * v, vec3(2.0, 4.0, 6.0)));
        CHECK(near(v * 2.0, vec3(2.0, 4.0, 6.0)));
        CHECK(near(0.5 * v, vec3(0.5, 1.0, 1.5)));
        // 负数标量
        CHECK(near(-1.0 * v, vec3(-1.0, -2.0, -3.0)));
    }

    // 4. 逐分量乘（注意：不是点积！）
    std::cout << "\n[4] 逐分量乘\n";
    {
        vec3 a(1.0, 2.0, 3.0);
        vec3 b(4.0, 5.0, 6.0);
        CHECK(near(a * b, vec3(4.0, 10.0, 18.0)));
    }

    // 5. 标量除
    std::cout << "\n[5] 标量除\n";
    {
        vec3 v(2.0, 4.0, 6.0);
        CHECK(near(v / 2.0, vec3(1.0, 2.0, 3.0)));
        CHECK(near(v / 4.0, vec3(0.5, 1.0, 1.5)));
    }

    // 6. 加减、一元负号
    std::cout << "\n[6] 加减与一元负号\n";
    {
        vec3 a(1.0, 2.0, 3.0);
        vec3 b(10.0, 20.0, 30.0);
        CHECK(near(a + b, vec3(11.0, 22.0, 33.0)));
        CHECK(near(b - a, vec3(9.0, 18.0, 27.0)));
        CHECK(near(-a, vec3(-1.0, -2.0, -3.0)));
        CHECK(near(a + (-a), vec3(0.0, 0.0, 0.0)));
    }

    // 7. 复合赋值 + 链式调用
    std::cout << "\n[7] 复合赋值与链式调用\n";
    {
        vec3 v(1.0, 2.0, 3.0);
        v += vec3(1.0, 1.0, 1.0);
        CHECK(near(v, vec3(2.0, 3.0, 4.0)));

        v -= vec3(0.5, 0.5, 0.5);
        CHECK(near(v, vec3(1.5, 2.5, 3.5)));

        v *= 2.0;
        CHECK(near(v, vec3(3.0, 5.0, 7.0)));

        v /= 2.0;
        CHECK(near(v, vec3(1.5, 2.5, 3.5)));

        // 链式：a += b += c 应能编译且结果正确
        vec3 a(1.0, 0.0, 0.0), b(0.0, 1.0, 0.0), c(0.0, 0.0, 1.0);
        a += b += c;
        // b 先变成 (0,1,1)，a 再变成 (1,1,1)
        CHECK(near(a, vec3(1.0, 1.0, 1.0)));
        CHECK(near(b, vec3(0.0, 1.0, 1.0)));
    }

    // 8. operator[] 可写版本
    std::cout << "\n[8] operator[] 可写\n";
    {
        vec3 v(0.0, 0.0, 0.0);
        v[0] = 7.0;
        v[1] = 8.0;
        v[2] = 9.0;
        CHECK(near(v, vec3(7.0, 8.0, 9.0)));
    }

    // 9. 点积
    std::cout << "\n[9] dot\n";
    {
        vec3 x(1.0, 0.0, 0.0);
        vec3 y(0.0, 1.0, 0.0);
        vec3 z(0.0, 0.0, 1.0);
        CHECK(approx(dot(x, y), 0.0));                 // 垂直
        CHECK(approx(dot(x, x), 1.0));                 // 单位向量自点积 = 1
        CHECK(approx(dot(vec3(1,2,3), vec3(4,5,6)), 32.0)); // 1*4+2*5+3*6 = 32
        CHECK(approx(dot(x, y), dot(y, x)));           // 对称性
        CHECK(approx(dot(-x, x), -1.0));               // 反向
    }

    // 10. 叉积
    std::cout << "\n[10] cross\n";
    {
        vec3 x(1.0, 0.0, 0.0);
        vec3 y(0.0, 1.0, 0.0);
        vec3 z(0.0, 0.0, 1.0);
        CHECK(near(cross(x, y), z));                   // x × y = z
        CHECK(near(cross(y, z), x));                   // y × z = x
        CHECK(near(cross(z, x), y));                   // z × x = y
        CHECK(near(cross(y, x), -z));                  // 反交换
        CHECK(near(cross(x, x), vec3(0,0,0)));         // 平行 → 零向量
        // 结果必须同时垂直于两个输入
        vec3 a(1, 2, 3), b(4, 5, 6);
        vec3 c = cross(a, b);
        CHECK(approx(dot(c, a), 0.0));
        CHECK(approx(dot(c, b), 0.0));
    }

    // 11. length / length_squared / unit_vector
    std::cout << "\n[11] 长度与单位化\n";
    {
        vec3 v(3.0, 4.0, 0.0);
        CHECK(approx(v.length_squared(), 25.0));
        CHECK(approx(v.length(), 5.0));

        vec3 u = unit_vector(v);
        CHECK(approx(u.length(), 1.0));                // 单位化后长度为 1
        CHECK(near(u, vec3(0.6, 0.8, 0.0)));           // 方向不变

        // 对已单位化的向量再单位化，应几乎不变
        vec3 u2 = unit_vector(u);
        CHECK(near(u2, u, 1e-12));
    }

    // 12. const 正确性：const 对象也能调访问器和只读运算
    std::cout << "\n[12] const 正确性\n";
    {
        const vec3 v(1.0, 2.0, 3.0);
        CHECK(approx(v.x(), 1.0));                     // const 成员函数
        CHECK(approx(v[0], 1.0));                      // const operator[]
        CHECK(approx(dot(v, v), 14.0));                // 自由函数接受 const&
        CHECK(approx(v.length(), std::sqrt(14.0)));
        // 下面这行如果取消注释，应当编译失败（const 对象不能调用可写 operator[]）
        // v[0] = 99.0;
    }

    // 13. operator<< 输出格式（三分量空格分隔）
    std::cout << "\n[13] operator<<\n";
    {
        vec3 v(1.5, -2.0, 3.25);
        std::cout << "  vec3(1.5,-2,3.25) 输出为: " << v << "\n";
        CHECK(true); // 只要上面能编译并打印，就说明重载可用
    }

    // ---------- 汇总 ----------
    std::cout << "\n==== 结果 ====\n";
    std::cout << "通过: " << g_pass << "  失败: " << g_fail << "\n";
    if (g_fail == 0) {
        std::cout << "全部通过，vec3.h 可以放心用了。\n";
        return 0;
    } else {
        std::cout << "有失败项，按上面的 [FAIL] 行号回去检查对应函数。\n";
        return 1;
    }
}