#define _WIN32_WINNT 0x0601
#include <iostream>
#include <fstream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd, client_fd;
    sockaddr_in serverAddr{}, clientAddr{};
    char buffer[4096];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.56.0");
    serverAddr.sin_port = htons(6000);

    bind(server_fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(server_fd, 3);
    std::cout << "File Transfer Server running on 127.0.56.0:6000\n";

    int clientSize = sizeof(clientAddr);
    client_fd = accept(server_fd, (sockaddr*)&clientAddr, &clientSize);

    // Read filename
    char filename[256] = {0};
    recv(client_fd, filename, sizeof(filename), 0);
    std::string newFile = "Server_";
    newFile += filename;

    // Read file size
    long long filesize;
    recv(client_fd, (char*)&filesize, sizeof(filesize), 0);

    std::ofstream outfile(newFile, std::ios::binary);
    long long received = 0;
    while (received < filesize) {
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        outfile.write(buffer, bytes);
        received += bytes;
    }
    outfile.close();

    const char* msg = "File received successfully!";
    send(client_fd, msg, strlen(msg), 0);
    std::cout << "Received file: " << filename << std::endl;

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
