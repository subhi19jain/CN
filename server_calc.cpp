#define _WIN32_WINNT 0x0601
#include <iostream>
#include <sstream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

double calculate(const std::string& expr) {
    std::stringstream ss(expr);
    double a, b;
    char op;
    ss >> a >> op >> b;
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
        default: return 0;
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd, client_fd;
    sockaddr_in serverAddr{}, clientAddr{};
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.46.0");
    serverAddr.sin_port = htons(7000);

    bind(server_fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(server_fd, 3);
    std::cout << "Calculator Server running on 127.0.46.0:7000\n";

    int clientSize = sizeof(clientAddr);
    client_fd = accept(server_fd, (sockaddr*)&clientAddr, &clientSize);

    recv(client_fd, buffer, sizeof(buffer), 0);
    std::string expr(buffer);
    double result = calculate(expr);

    std::string reply = "Result: " + std::to_string(result);
    send(client_fd, reply.c_str(), reply.size(), 0);

    std::cout << "Calculated: " << expr << " = " << result << std::endl;

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
