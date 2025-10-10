#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.46.0");
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Server waiting for connection...\n";

    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    read(new_socket, buffer, 1024);
    std::cout << "Client says: " << buffer << std::endl;

    std::string reply = "Hello from Server!";
    send(new_socket, reply.c_str(), reply.length(), 0);

    close(new_socket);
    close(server_fd);
    return 0;
}
