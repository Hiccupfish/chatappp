#define _WIN32_WINNT 0x0501
#include "common.h"
#include <WS2tcpip.h>
#include <winsock2.h>
#include <iostream>
#include <string>

SOCKET create_client_socket(const char* server_address, const char* port) {
    struct addrinfo hints, *result = NULL;
    SOCKET client_socket = INVALID_SOCKET;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(server_address, port, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        cleanup_winsock();
        exit(1);
    }

    client_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        cleanup_winsock();
        exit(1);
    }

    iResult = connect(client_socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        freeaddrinfo(result);
        cleanup_winsock();
        exit(1);
    }

    freeaddrinfo(result);

    return client_socket;
}

void run_client() {
    const char* server_address = "127.0.0.1";
    const char* DEFAULT_PORT = "27015";

    initialize_winsock();

    SOCKET client_socket = create_client_socket(server_address, DEFAULT_PORT);
    send_message(client_socket, 'J', "User joined");
    send_message(client_socket, 'M', "Hello, Server!");

    while (true) {
        receive_message(client_socket);
    }

    closesocket(client_socket);
    cleanup_winsock();
}
