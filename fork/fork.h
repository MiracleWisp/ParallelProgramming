//
// Created by s-uch on 29.09.2021.
//

#ifndef PARALLEL_PROGRAMMING_CREATE_PROCESS_H
#define PARALLEL_PROGRAMMING_CREATE_PROCESS_H

int start_server(unsigned int threads_count);

point find_max_fork(double func(double), unsigned int threads_count);

void process_message(int listening_socket, point *responses);

void send_result(int id, point result);

point find_max_in_range(double func(double), double start, double end);

#endif //PARALLEL_PROGRAMMING_CREATE_PROCESS_H
