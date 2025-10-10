#include <iostream>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.46.0", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    std::ofstream outfile("rishabh.txt", std::ios::binary);

    int bytes;
    while ((bytes = read(sock, buffer, 1024)) > 0) {
        outfile.write(buffer, bytes);
    }

    std::cout << "File received.\n";
    outfile.close();
    close(sock);
    return 0;
}
