//
// Created by s-uch on 28.09.2021.
//

#include <pthread.h>
#include <iostream>
#include <thread>
#include "../utils.h"
#include <cassert>

using namespace std;

void *thread_func(void *arg) {
    auto *data = (thread_data *) arg;
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
    return nullptr;
}

point find_max_pthread_create(double func(double), unsigned int threads_count) {
    pthread_t thread_ids[threads_count];
    thread_data thread_data_list[threads_count];
    int rc;
    for (int i = 0; i < threads_count; ++i) {
        double part = RANGE / threads_count;
        thread_data_list[i] = thread_data(func, part * i, part * (i + 1));
        rc = pthread_create(
                &thread_ids[i],
                nullptr,
                thread_func,
                &thread_data_list[i]
        );
        assert (0 == rc);
    }

    for (int i = 0; i < threads_count; ++i) {
        rc = pthread_join(thread_ids[i], nullptr);
        assert (0 == rc);
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
    cout << "pthread_create()" << endl;
    auto start = chrono::steady_clock::now();
    auto point = find_max_pthread_create(func, processor_count);
    auto end = chrono::steady_clock::now();
    cout << "X: " << point.x << " Y: " << point.y << endl;
    cout << "Duration: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    return 0;
}
