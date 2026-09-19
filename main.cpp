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
    calc_result* result = new calc_result{calc(args->tests, args->seed)};
    return result;
}


double area(size_t threads, size_t tests) {
    size_t perThread = tests / threads;

    std::vector<pthread_t> threadsVec;
    std::vector<calc_args*> threadArguments;
    threadsVec.reserve(threads);
    threadArguments.reserve(threads);
    size_t hit = 0;

    for (size_t i = 0; i < threads; i++) {
        pthread_t th;
        auto * args = new calc_args{perThread, i};
        int err = pthread_create(&th, nullptr, calc_wrappper, args);
        threadsVec.push_back(th);
        threadArguments.push_back(args);
    }
    for (size_t i = 0; i < threads; i++) {
        pthread_t th =  threadsVec[i];
        void* result;
        int err =pthread_join(th, &result);
        auto * res = static_cast<calc_result*>(result);
        hit += res->tests;

        delete res;
        delete threadArguments[i];
    }
    return static_cast<double>(hit) / static_cast<double>(tests) * 4.0;
}


int main() {
    std::cout << area(4, 200) << std::endl;
}
