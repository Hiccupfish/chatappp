#include "common.h"
#include <iostream>

void initialize_winsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
}

void cleanup_winsock() {
    WSACleanup();
}

void send_message(SOCKET socket, char command, const std::string &message) {
    int message_length = message.length();
    int total_length = 1 + sizeof(int) + message_length;

    char *buffer = new char[total_length];

    buffer[0] = command;
    memcpy(buffer + 1, &message_length, sizeof(int));
    memcpy(buffer + 1 + sizeof(int), message.c_str(), message_length);

    int sent = send(socket, buffer, total_length, 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        cleanup_winsock();
        delete[] buffer;
        exit(1);
    }

    delete[] buffer;
}

void receive_message(SOCKET socket) {  // Ensure the signature matches the declaration
    char command;
    int message_length;
    char recvbuf[512];

    int result = recv(socket, &command, 1, 0);
    if (result <= 0) {
        if (result == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        cleanup_winsock();
        exit(1);
    }

    result = recv(socket, (char*)&message_length, sizeof(int), 0);
    if (result <= 0) {
        if (result == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        cleanup_winsock();
        exit(1);
    }

    result = recv(socket, recvbuf, message_length, 0);
    if (result <= 0) {
        if (result == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        cleanup_winsock();
        exit(1);
    }

    recvbuf[result] = '\0';
    std::cout << "Command: " << command << ", Message: " << recvbuf << std::endl;
}
