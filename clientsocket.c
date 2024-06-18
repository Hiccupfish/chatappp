
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

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

void communicate_with_server(SOCKET client_socket) {
    const char* sendbuf = "Hello, Server!";
    char recvbuf[512];
    int result;

    // Send a message to the server
    result = send(client_socket, sendbuf, (int)strlen(sendbuf), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        cleanup_winsock();
        exit(1);
    }

    std::cout << "Bytes sent: " << result << std::endl;

    // Receive data from the server
    result = recv(client_socket, recvbuf, 512, 0);
    if (result > 0) {
        std::cout << "Bytes received: " << result << std::endl;
        recvbuf[result] = '\0';
        std::cout << "Server says: " << recvbuf << std::endl;
    } else if (result == 0) {
        std::cout << "Connection closed" << std::endl;
    } else {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    }
}

int main() {
    const char* server_address = "127.0.0.1"; // Server IP address
    const char* DEFAULT_PORT = "27015"; // Port used by the server

    initialize_winsock();

    SOCKET client_socket = create_client_socket(server_address, DEFAULT_PORT);
    communicate_with_server(client_socket);

    // Cleanup
    closesocket(client_socket);
    cleanup_winsock();

    return 0;
}

