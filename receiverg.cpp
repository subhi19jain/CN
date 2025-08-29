// receiver.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.56");
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    std::cout << "[RECEIVER] Waiting on 127.0.0.56:8080...\n";

    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        return -1;
    }

    std::cout << "[RECEIVER] Connected to sender.\n";

    int expected_seq = 0;
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes <= 0) break;

        int recv_seq;
        sscanf(buffer, "SEQ %d", &recv_seq);

        std::cout << "[RECEIVER] Received SEQ " << recv_seq << "\n";

        if (recv_seq == expected_seq) {
            std::string ack = "ACK " + std::to_string(expected_seq);
            send(new_socket, ack.c_str(), ack.length(), 0);
            std::cout << "[RECEIVER] Sent ACK " << expected_seq << "\n";
            expected_seq++;
            if (expected_seq == 8) {  // Stop after receiving 8 packets
                std::cout << "[RECEIVER] All packets received.\n";
                break;
            }
        } else {
            // Send duplicate ACK for last in-order packet received
            std::string dupAck = "ACK " + std::to_string(expected_seq - 1);
            send(new_socket, dupAck.c_str(), dupAck.length(), 0);
            std::cout << "[RECEIVER] Sent duplicate ACK " << expected_seq - 1 << "\n";
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
