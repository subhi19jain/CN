#define _WIN32_WINNT 0x0601
#include <iostream>
#include <fstream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock;
    sockaddr_in servAddr{};
    char buffer[4096];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.56.0");
    servAddr.sin_port = htons(6000);

    connect(sock, (sockaddr*)&servAddr, sizeof(servAddr));

    std::string filepath;
    std::cout << "Enter file path to send: ";
    getline(std::cin, filepath);

    std::ifstream infile(filepath, std::ios::binary);
    infile.seekg(0, std::ios::end);
    long long filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);

    send(sock, filename.c_str(), filename.size() + 1, 0);
    send(sock, (char*)&filesize, sizeof(filesize), 0);

    while (!infile.eof()) {
        infile.read(buffer, sizeof(buffer));
        send(sock, buffer, infile.gcount(), 0);
    }
    infile.close();

    char reply[1024] = {0};
    recv(sock, reply, sizeof(reply), 0);
    std::cout << reply << std::endl;

    closesocket(sock);
    WSACleanup();
    return 0;
}
