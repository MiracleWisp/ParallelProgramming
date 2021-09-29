//
// Created by s-uch on 27.09.2021.
//

#include <limits>
#include <iostream>
#include <chrono>
#include "../function.h"

using namespace std;

point find_max_brute(double (*func)(double)) {
    point res = {
            0,
            -numeric_limits<double>::infinity()
    };
    double x = 0;
    while (x <= RANGE) {
        double y = func(x);
        if (y > res.y) {
            res.y = y;
            res.x = x;
        }
        x += 1E-4;
    }
    return res;
}

int main() {
    cout << "BRUTE" << endl;
    auto start = chrono::steady_clock::now();
    auto point = find_max_brute(func);
    auto end = chrono::steady_clock::now();
    cout << "X: " << point.x << " Y: " << point.y << endl;
    cout << "Duration: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    return 0;
}