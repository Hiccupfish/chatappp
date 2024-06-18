#ifndef COMMON_H
#define COMMON_H

#include <winsock2.h>
#include <string>

void initialize_winsock();
void cleanup_winsock();
void send_message(SOCKET socket, char command, const std::string &message);
void receive_message(SOCKET socket);  // Ensure the signature is correct

#endif // COMMON_H
