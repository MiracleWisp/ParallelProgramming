//
// Created by s-uch on 06.10.2021.
//

#include <mpi.h>
#include <iostream>
#include "mpi_all.h"
#include "../utils.h"

#define MAIN_ID 0

using namespace std;

void reduce_max(void *inputBuffer, void *outputBuffer, int *len, MPI_Datatype *datatype) {
    auto *input = (point *) inputBuffer;
    auto *output = (point *) outputBuffer;

    if (input->y > output->y) {
        output->y = input->y;
        output->x = input->x;
    }
}

int main(int argc, char *argv[]) {

    int id, processes_count;
    double cpu_time_start, cpu_time_end;

    MPI_Init(&argc, &argv);
    cpu_time_start = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    /* Create custom type */
    MPI_Datatype dt_point;
    MPI_Type_contiguous(2, MPI_DOUBLE, &dt_point);
    MPI_Type_commit(&dt_point);
    MPI_Op operation;
    MPI_Op_create(&reduce_max, 1, &operation);

    /* In case of single process execution */
    if (processes_count == 1) {
        point res = find_max_in_range(func, 0, RANGE);
        cpu_time_end = MPI_Wtime();
        cout << "MPI" << endl;
        cout << "X: " << res.x << " Y: " << res.y << endl;
        cout << "Duration: " << (cpu_time_end - cpu_time_start) * 1000 << "ms" << endl;
        return 0;
    }


    double part = RANGE / processes_count;
    point max_point = find_max_in_range(func, part * (id - 1), part * id);
    point result{};
    MPI_Reduce(&max_point, &result, 1, dt_point, operation, 0, MPI_COMM_WORLD);
    cpu_time_end = MPI_Wtime();

    if (id == MAIN_ID) {
        /* MAIN */
        cout << "MPI" << endl;
        cout << "X: " << result.x << " Y: " << result.y << endl;
        cout << "Duration: " << (cpu_time_end - cpu_time_start) * 1000 << "ms" << endl;
    }


    MPI_Finalize();

    return 0;
}

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
