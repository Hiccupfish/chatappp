#define _WIN32_WINNT 0x0501
#include "common.h"
#include <WS2tcpip.h>
#include <winsock2.h>
#include <iostream>

SOCKET create_listen_socket(const char* port) {
    struct addrinfo hints, *result = NULL;
    SOCKET listen_socket = INVALID_SOCKET;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        cleanup_winsock();
        exit(1);
    }

    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        cleanup_winsock();
        exit(1);
    }

    iResult = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(listen_socket);
        cleanup_winsock();
        exit(1);
    }

    freeaddrinfo(result);

    iResult = listen(listen_socket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        cleanup_winsock();
        exit(1);
    }

    return listen_socket;
}

void run_server() {
    const char* DEFAULT_PORT = "27015";

    SOCKET listen_socket = create_listen_socket(DEFAULT_PORT);
    SOCKET client_socket;

    std::cout << "Waiting for client to connect..." << std::endl;

    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        cleanup_winsock();
        return;
    }

    std::cout << "Client connected" << std::endl;

    while (true) {
        receive_message(client_socket);
    }

    closesocket(client_socket);
    closesocket(listen_socket);
    cleanup_winsock();
}
