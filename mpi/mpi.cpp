//
// Created by s-uch on 01.10.2021.
//


#include <mpi.h>
#include <iostream>
#include "mpi.h"
#include "../utils.h"

#define MAIN_ID 0

using namespace std;

int main(int argc, char *argv[]) {

    int id, processes_count, workers_count;
    double cpu_time_start, cpu_time_end;

    MPI_Init(&argc, &argv);
    cpu_time_start = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    workers_count = processes_count - 1;

    /* In case of single process execution */
    if (processes_count == 1) {
        point res = find_max_in_range(func, 0, RANGE);
        cpu_time_end = MPI_Wtime();
        cout << "MPI" << endl;
        cout << "X: " << res.x << " Y: " << res.y << endl;
        cout << "Duration: " << (cpu_time_end - cpu_time_start) * 1000 << "ms" << endl;
        return 0;
    }

    if (id == MAIN_ID) {
        /* MAIN */
        point res = {
                -1,
                -numeric_limits<double>::infinity()
        };
        for (int i = 1; i < processes_count; ++i) {
            point p = process_message();
            if (p.y > res.y) {
                res = p;
            }
        }
        cpu_time_end = MPI_Wtime();
        cout << "MPI" << endl;
        cout << "X: " << res.x << " Y: " << res.y << endl;
        cout << "Duration: " << (cpu_time_end - cpu_time_start) * 1000 << "ms" << endl;
    } else {
        /* Worker */
        double part = RANGE / workers_count;
        point res = find_max_in_range(func, part * (id - 1), part * id);
        send_result(res);
    }

    MPI_Finalize();

    return 0;
}

point process_message() {
    MPI_Status status;
    double message[2];
    MPI_Recv(message, 2, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    point point = {
            message[0],
            message[1]
    };
    return point;
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

void send_result(point result) {
    double message[2]{result.x, result.y};
    MPI_Send(message, 2, MPI_DOUBLE, MAIN_ID, 0, MPI_COMM_WORLD);
}