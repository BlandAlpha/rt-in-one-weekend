#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
  public:
    double min, max;

    // 注意这种写法，是成员初始化列表，如果在里面赋值就会先构造再赋值，在这里就是构造的同时赋值
    interval() : min(+infinity), max(-infinity) {}

    interval(double min, double max) : min{min}, max(max) {}

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;

};

// 故意写反
const interval interval::empty    = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif