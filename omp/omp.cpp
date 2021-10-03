//
// Created by s-uch on 03.10.2021.
//

#include <iostream>
#include <omp.h>
#include "../utils.h"
#include <chrono>

using namespace std;

#pragma omp declare reduction(maximum : point : omp_out = omp_in.y > omp_out.y ? omp_in : omp_out)

point find_max_in_range(double func(double), double start, double end) {


    point result{
            -1,
            -numeric_limits<double>::infinity()
    };
    double x = start;
    while (x <= end) {
        double y = func(x);
        if (y > result.y) {
            result.y = y;
            result.x = x;
        }
        x += 1E-4;
    }
    return result;
}


int main() {
    cout << "OpenMP" << endl;
    auto start = chrono::steady_clock::now();

    point res{
            0,
            -numeric_limits<double>::infinity()
    };

#pragma omp parallel default(none) shared(RANGE) reduction(maximum: res)
    {
        int threads_count = omp_get_num_threads();
        int thread_id = omp_get_thread_num();
        double part = RANGE / threads_count;
        res = find_max_in_range(func, part * thread_id, part * (thread_id + 1));
    }

    auto end = chrono::steady_clock::now();
    cout << "X: " << res.x << " Y: " << res.y << endl;
    cout << "Duration: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    return 0;
}

