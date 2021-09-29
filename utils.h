//
// Created by s-uch on 28.09.2021.
//

#ifndef PARALLEL_PROGRAMMING_UTILS_H
#define PARALLEL_PROGRAMMING_UTILS_H

#include <functional>
#include "function.h"

struct thread_data {
    std::function<double(double)> func;
    double x_start{};
    double x_end{};
    point res{};

    thread_data() = default;

    thread_data(std::function<double(double)> func, double x_start, double x_end) : func(std::move(func)),
                                                                                    x_start(x_start),
                                                                                    x_end(x_end) {}
};

#endif //PARALLEL_PROGRAMMING_UTILS_H
