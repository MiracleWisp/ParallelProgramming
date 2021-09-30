//
// Created by s-uch on 29.09.2021.
//

//
// Created by s-uch on 27.09.2021.
//

#include <iostream>
#include <sstream>
#include <thread>
#include <string>
#include <winsock2.h>
#include "windows.h"
#include "../utils.h"
#include "create_process.h"

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;

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
    WSADATA wsa;
    SOCKET ConnectSocket;
    struct sockaddr_in server{};

    WSAStartup(MAKEWORD(2, 2), &wsa);


    //Create a socket
    (ConnectSocket = socket(AF_INET, SOCK_STREAM, 0));


    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(DEFAULT_PORT));

    //Connect to remote server
    connect(ConnectSocket, (struct sockaddr *) &server, sizeof(server));


    if (ConnectSocket == INVALID_SOCKET) {
        WSACleanup();
    }

    // Send an initial buffer
    int iResult;
    char sendbuf[DEFAULT_BUFLEN];
    sprintf(sendbuf, "%d %f %f", id, result.x, result.y);
    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
    }


    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        /* PARENT */
        const auto processor_count = thread::hardware_concurrency();
        cout << "CreateProcess()" << endl;
        auto start = chrono::steady_clock::now();
        auto point = find_max_create_process(processor_count, argv[0]);
        auto end = chrono::steady_clock::now();
        cout << "X: " << point.x << " Y: " << point.y << endl;
        cout << "Duration: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    } else {
        /* CHILD */
        point result = find_max_in_range(func, stod(argv[1]), stod(argv[2]));
        send_result(atoi(argv[0]), result);
    }

    return 0;
}

SOCKET start_server(unsigned int threads_count) {
    WSADATA wsa;
    SOCKET listening_socket;
    struct sockaddr_in server{};

    WSAStartup(MAKEWORD(2, 2), &wsa);


    //Create a socket
    (listening_socket = socket(AF_INET, SOCK_STREAM, 0));


    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(DEFAULT_PORT));

    //Bind
    bind(listening_socket, (struct sockaddr *) &server, sizeof(server));


    //Listen to incoming connections
    listen(listening_socket, threads_count);
    /*
        closesocket(s);
        WSACleanup();
    */
    return listening_socket;
}


point find_max_create_process(unsigned int threads_count, char *command) {

    SOCKET socket = start_server(threads_count);

    point responses[threads_count];
    for (int i = 0; i < threads_count; ++i) {
        double part = RANGE / threads_count;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        string args_str = to_string(i) + " " + to_string(part * i) + " " + to_string(part * (i + 1));
        char *args = new char[args_str.length() + 1];
        strcpy(args, args_str.c_str());
        CreateProcessA(command,
                       args,
                       nullptr,
                       nullptr,
                       FALSE,
                       0,
                       nullptr,
                       nullptr,
                       &si,
                       &pi);
    }

    int response_count = 0;
    point res = {
            0,
            -numeric_limits<double>::infinity()
    };

    while (response_count < threads_count) {
        process_message(socket, responses);
        response_count++;
    }

    for (int i = 0; i < threads_count; ++i) {
        if (responses[i].y > res.y) {
            res.y = responses[i].y;
            res.x = responses[i].x;
        }
    }

    return res;
}


void process_message(SOCKET listening_socket, point *responses) {
    SOCKET accept_socket;
    struct sockaddr_in client{};
    int c;

    c = sizeof(struct sockaddr_in);
    accept_socket = accept(listening_socket, (struct sockaddr *) &client, &c);

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    recv(accept_socket, recvbuf, recvbuflen, 0);

    string response_str(recvbuf);
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
