#include <future>
#include <iostream>
#include <random>
#include <thread>
// входит ли точка в круг в координатах (1,1)

struct p_t {
    double x, y;
};

p_t center = {1, 1};
static double radius = 1;

bool isInside(p_t point, p_t center, double r) {
    double xdiff = point.x - center.x;
    double ydiff = point.y - center.y;

    return r * r >= xdiff * xdiff + ydiff * ydiff;
}


size_t calc(size_t tests, size_t seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    size_t total = 0;

    for (size_t i = 0; i < tests; ++i) {
        double rx = dis(gen);
        double ry = dis(gen);
        p_t random_p_t = {rx, ry};
        total += isInside(random_p_t, center, radius);
    }
    return total;
}

double area(size_t threads, size_t tests) {
    size_t per_thread = tests  / threads;

    std::vector<std::future<size_t>> f;
    f.reserve(threads);

    size_t hit = 0;
    for (size_t i = 0; i < tests; i++) {
        f.push_back(std::async(std::launch::async, calc, per_thread, i));
    }
    for (std::future<size_t>& item : f) {
        hit += item.get();
    }
    return static_cast<double>(hit) / static_cast<double>(tests);
}


int main() {
    std::cout << area(4, 200) << std::endl;
}
