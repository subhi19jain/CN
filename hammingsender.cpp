#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void generateHammingCode(int data[], int code[]) {
    code[2] = data[0]; 
    code[4] = data[1];
    code[5] = data[2];
    code[6] = data[3];

    code[0] = data[0] ^ data[1] ^ data[3]; // p1
    code[1] = data[0] ^ data[2] ^ data[3]; // p2
    code[3] = data[1] ^ data[2] ^ data[3]; // p3

    int overall_parity = 0;
    for (int i = 0; i < 7; ++i) {
        overall_parity ^= code[i];
    }
    code[7] = overall_parity;
}

void induceError(int code[]) {
    srand(time(0));
    int choice = rand() % 3;

    switch (choice) {
        case 0:
            cout << "[INFO] No error induced.\n";
            break;

        case 1: {
            int pos = rand() % 8;
            code[pos] ^= 1;
            cout << "[INFO] Single-bit error induced at position: " << pos << "\n";
            break;
        }

        case 2: {
            int pos1 = rand() % 8;
            int pos2;
            do {
                pos2 = rand() % 8;
            } while (pos2 == pos1);
            code[pos1] ^= 1;
            code[pos2] ^= 1;
            cout << "[INFO] Double-bit error induced at positions: " << pos1 << " and " << pos2 << "\n";
            break;
        }
    }
}

int main() {
    int data[4], code[8];
    cout << "Enter 4 bits (0 or 1): ";
    for (int i = 0; i < 4; ++i)
        cin >> data[i];

    generateHammingCode(data, code);
    induceError(code);

    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "[ERROR] Socket creation failed.\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.56", &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "[ERROR] Connection Failed\n";
        return -1;
    }

    send(sockfd, code, sizeof(code), 0);
    cout << "[SENT] Codeword: ";
    for (int i = 0; i < 8; ++i)
        cout << code[i] << " ";
    cout << "\n";

    close(sockfd);
    return 0;
}
