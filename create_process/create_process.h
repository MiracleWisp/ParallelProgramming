//
// Created by s-uch on 29.09.2021.
//

#ifndef PARALLEL_PROGRAMMING_CREATE_PROCESS_H
#define PARALLEL_PROGRAMMING_CREATE_PROCESS_H

#include "../utils.h"
#include <winsock2.h>

SOCKET start_server(unsigned int threads_count);

point find_max_create_process(unsigned int threads_count, char *command);

void process_message(SOCKET listening_socket, point *responses);

void send_result(int id, point result);

point find_max_in_range(double func(double), double start, double end);

#endif //PARALLEL_PROGRAMMING_CREATE_PROCESS_H
