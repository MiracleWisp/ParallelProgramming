//
// Created by s-uch on 29.09.2021.
//

#ifndef PARALLEL_PROGRAMMING_CREATE_PROCESS_H
#define PARALLEL_PROGRAMMING_CREATE_PROCESS_H

SOCKET start_server(unsigned int threads_count);

point find_max_create_thread(unsigned int threads_count, char *command);

void process_message(SOCKET listening_socket, point *responses);

#endif //PARALLEL_PROGRAMMING_CREATE_PROCESS_H
