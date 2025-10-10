#define _WIN32_WINNT 0x0601
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock;
    sockaddr_in servAddr{};
    char buffer[1024] = {0};
    std::string message;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.56.0");
    servAddr.sin_port = htons(5000);

    if (connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    std::cout << "Enter message to server: ";
    getline(std::cin, message);
    send(sock, message.c_str(), message.size(), 0);

    recv(sock, buffer, sizeof(buffer), 0);
    std::cout << "Server replies: " << buffer << std::endl;

    closesocket(sock);
    WSACleanup();
    return 0;
}
