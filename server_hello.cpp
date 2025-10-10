#define _WIN32_WINNT 0x0601
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd, client_fd;
    sockaddr_in serverAddr{}, clientAddr{};
    char buffer[1024] = {0};
    const char* reply = "Hello from Server!";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.56.0");
    serverAddr.sin_port = htons(5000);

    if (bind(server_fd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    listen(server_fd, 3);
    std::cout << "Hello Server running on 127.0.56.0:5000\n";

    int clientSize = sizeof(clientAddr);
    client_fd = accept(server_fd, (sockaddr*)&clientAddr, &clientSize);

    recv(client_fd, buffer, sizeof(buffer), 0);
    std::cout << "Client says: " << buffer << std::endl;

    send(client_fd, reply, strlen(reply), 0);
    std::cout << "Sent reply to client.\n";

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
