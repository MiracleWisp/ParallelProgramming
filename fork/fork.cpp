//
// Created by s-uch on 30.09.2021.
//

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <cstring>
#include "../utils.h"
#include "fork.h"
#include <arpa/inet.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;

int main() {
    const auto processor_count = thread::hardware_concurrency();
    cout << "fork()" << endl;
    auto start = chrono::steady_clock::now();
    auto point = find_max_fork(func, processor_count);
    auto end = chrono::steady_clock::now();
    cout << "X: " << point.x << " Y: " << point.y << endl;
    cout << "Duration: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    return 0;
}

int start_server(unsigned int threads_count) {
    int sockfd, portno;
    struct sockaddr_in serv_addr{};
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(DEFAULT_PORT);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(sockfd, threads_count);
    return sockfd;
}

point find_max_fork(double func(double), unsigned int threads_count) {

    int sockfd = start_server(threads_count);

    point responses[threads_count];
    pid_t pids[threads_count];
    double part = RANGE / threads_count;
    for (int i = 0; i < threads_count; ++i) {
        pids[i] = fork();
        if (pids[i]) {
            /* PARENT */
        } else {
            /* CHILD */
            point result = find_max_in_range(func, part * i, part * (i + 1));
            send_result(i, result);
            exit(0);
        }
    }

    int response_count = 0;
    point res = {
            0,
            -numeric_limits<double>::infinity()
    };

    while (response_count < threads_count) {
        process_message(sockfd, responses);
        response_count++;
    }

    close(sockfd);

    for (int i = 0; i < threads_count; ++i) {
        if (responses[i].y > res.y) {
            res.y = responses[i].y;
            res.x = responses[i].x;
        }
    }

    return res;
}

void process_message(int listening_socket, point *responses) {
    struct sockaddr_in cli_addr{};
    socklen_t clilen;
    clilen = sizeof(cli_addr);

    int accept_socket = accept(listening_socket, (struct sockaddr *) &cli_addr, &clilen);

    char buffer[DEFAULT_BUFLEN];
    bzero(buffer, DEFAULT_BUFLEN);
    read(accept_socket, buffer, DEFAULT_BUFLEN);
    close(accept_socket);

    string response_str(buffer);
    istringstream iss(response_str);

    int id = 0;
    iss >> id;
    double x, y;
    iss >> x;
    iss >> y;

    point response{
            x,
            y
    };

    responses[id] = response;
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

void send_result(int id, point result) {
    int sockfd, portno;
    struct sockaddr_in serv_addr{};

    char buffer[DEFAULT_BUFLEN];

    portno = atoi(DEFAULT_PORT);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(DEFAULT_PORT));
    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    bzero(buffer, DEFAULT_BUFLEN);
    sprintf(buffer, "%d %f %f", id, result.x, result.y);
    write(sockfd, buffer, strlen(buffer));
    close(sockfd);
}
