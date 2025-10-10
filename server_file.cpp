#include <iostream>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.46.0");
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);

    std::cout << "Waiting for client...\n";
    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    std::ifstream file("rishabh.txt", std::ios::binary);
    if (!file) {
        std::cerr << "File not found.\n";
        close(new_socket);
        return 1;
    }

    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        send(new_socket, buffer, file.gcount(), 0);
    }

    std::cout << "File sent.\n";
    file.close();
    close(new_socket);
    close(server_fd);
    return 0;
}
