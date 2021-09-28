#include <cmath>
#include "function.h"

using namespace std;

// cos(3*x^4) * sin(5*x^2) * sin(5x)^2
double func(double x) {
    return cos(3 * pow(x, 4)) * sin(5 * pow(x, 2)) * pow(sin(5 * x), 2);
}
