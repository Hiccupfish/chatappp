#define _WIN32_WINNT 0x0501

#include <iostream>
#include "common.h"
#include <winsock2.h>
#include <WS2tcpip.h>

void run_server();
void run_client();

int main() {
    int choice;
    bool winsock_initialized = false;

    std::cout << "Choose mode:\n1. Run Server\n2. Run Client\n";
    std::cin >> choice;

    if (choice == 1) {
        initialize_winsock();
        winsock_initialized = true;
        run_server();

        // After running the server, prompt to run the client
        std::cout << "Server is running. Press any key to start the client...";
        std::cin.ignore(); // Ignore previous newline character
        std::cin.get(); // Wait for user to press Enter
        run_client();
    } else if (choice == 2) {
        initialize_winsock();
        winsock_initialized = true;
        run_client();
    } else {
        std::cerr << "Invalid choice" << std::endl;
    }

    if (winsock_initialized) {
        cleanup_winsock();
    }

    return 0;
}
