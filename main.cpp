#include <future>
#include <iostream>
#include <random>
#include <thread>

struct p_t {
    double x, y;
};

p_t center = {1, 1};
static double radius = 1;

struct calc_args {
    size_t tests;
    size_t seed;
};

struct calc_result {
    size_t tests;
};


bool isInside(p_t point, p_t center, double r) {
    double xdiff = point.x - center.x;
    double ydiff = point.y - center.y;

    return r * r >= xdiff * xdiff + ydiff * ydiff;
}


size_t calc(size_t tests, size_t seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(0.0, 2.0);

    size_t total = 0;

    for (size_t i = 0; i < tests; ++i) {
        double rx = dis(gen);
        double ry = dis(gen);
        p_t random_p_t = {rx, ry};
        total += isInside(random_p_t, center, radius);
    }
    return total;
}

void *calc_wrappper(void *data) {
    calc_args *args = static_cast<calc_args *>(data);
    calc_result *result = new calc_result{calc(args->tests, args->seed)};
    return result;
}

struct CalcWrapper {
    calc_args *args;
    calc_result *result;
    pthread_t th;

    CalcWrapper(size_t tests, size_t seed) : args(new calc_args{tests, seed}), result(nullptr), th(0) {
        int err = pthread_create(&th, nullptr, calc_wrappper, args);
    }

    calc_result get() {
        void *outputPtr;
        int err = pthread_join(th, &outputPtr);
        result = static_cast<calc_result *>(outputPtr);
        return *result;
    }

    ~CalcWrapper() {
        delete args;
        delete result;
    }

    CalcWrapper(CalcWrapper &) = delete;

    CalcWrapper(CalcWrapper &&other) noexcept : args(other.args), result(other.result), th(other.th) {
        other.args = nullptr;
        other.result = nullptr;
        other.th = 0;
    };

    CalcWrapper &operator=(CalcWrapper &) = delete;

    CalcWrapper &operator=(CalcWrapper &&) = delete;
};

double area(size_t threads, size_t tests) {
    size_t perThread = tests / threads;

    std::vector<CalcWrapper> threadsVec;
    size_t hit = 0;

    for (size_t i = 0; i < threads; i++) {
        threadsVec.emplace_back(perThread, i);
    }
    for (size_t i = 0; i < threads; i++) {
        hit += threadsVec[i].get().tests;
    }
    return static_cast<double>(hit) / static_cast<double>(tests) * 4.0;
}


int main() {
    std::cout << area(4, 200) << std::endl;
}
