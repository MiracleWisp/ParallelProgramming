//
// Created by s-uch on 30.09.2021.
//

#include <iostream>
#include <thread>
#include "../utils.h"

using namespace std;

void thread_func(thread_data *data) {
    double max = -numeric_limits<double>::infinity();
    double x = data->x_start;
    double max_x;
    while (x <= data->x_end) {
        double y = data->func(x);
        if (y > max) {
            max = y;
            max_x = x;
        }
        x += 1E-4;
    }
    data->res.x = max_x;
    data->res.y = max;
}

point find_max_std_thread(double func(double), unsigned int threads_count) {
    thread threads[threads_count];
    thread_data thread_data_list[threads_count];
    for (int i = 0; i < threads_count; ++i) {
        double part = RANGE / threads_count;
        thread_data_list[i] = thread_data(func, part * i, part * (i + 1));
        threads[i] = thread(thread_func, &thread_data_list[i]);
    }

    for (int i = 0; i < threads_count; ++i) {
        threads[i].join();
    }

    point res = {
            0,
            -numeric_limits<double>::infinity()
    };

    for (int i = 0; i < threads_count; ++i) {
        if (thread_data_list[i].res.y > res.y) {
            res.y = thread_data_list[i].res.y;
            res.x = thread_data_list[i].res.x;
        }
    }

    return res;
}

int main() {
    const auto processor_count = thread::hardware_concurrency();
    cout << "std::thread" << endl;
    auto start = chrono::steady_clock::now();
    auto point = find_max_std_thread(func, processor_count);
    auto end = chrono::steady_clock::now();
    cout << "X: " << point.x << " Y: " << point.y << endl;
    cout << "Duration: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    return 0;
}