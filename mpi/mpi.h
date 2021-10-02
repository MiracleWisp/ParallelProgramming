//
// Created by s-uch on 03.10.2021.
//

#ifndef PARALLEL_PROGRAMMING_MPI_H
#define PARALLEL_PROGRAMMING_MPI_H

#include "../utils.h"

point process_message();

void send_result(point result);

point find_max_in_range(double func(double), double start, double end);

#endif //PARALLEL_PROGRAMMING_MPI_H
