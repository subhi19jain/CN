#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int calculateSyndrome(int code[]) {
    int p1 = code[0] ^ code[2] ^ code[4] ^ code[6];
    int p2 = code[1] ^ code[2] ^ code[5] ^ code[6];
    int p3 = code[3] ^ code[4] ^ code[5] ^ code[6];
    return p3 * 4 + p2 * 2 + p1;
}

bool checkOverallParity(int code[]) {
    int parity = 0;
    for (int i = 0; i < 8; ++i) {
        parity ^= code[i];
    }
    return parity == 0;
}

void printBits(const int code[], const string& label) {
    cout << label;
    for (int i = 0; i < 8; ++i) {
        cout << code[i] << " ";
    }
    cout << "\n";
}

void checkAndCorrect(int code[]) {
    int syndrome = calculateSyndrome(code);
    bool overall_parity_ok = checkOverallParity(code);

    switch (syndrome) {
        case 0:
            if (overall_parity_ok) {
                cout << "[INFO] No error detected.\n";
            } else {
                cout << "[ERROR] Error detected in overall parity bit. Correcting...\n";
                code[7] ^= 1; // Correct parity bit
            }
            break;

        case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            if (overall_parity_ok) {
                cout << "[WARNING] Double bit error detected. Unable to correct.\n";
            } else {
                cout << "[INFO] Single bit error detected at position " << syndrome - 1 << ". Correcting...\n";
                code[syndrome - 1] ^= 1;  // Correct error bit
            }
            break;

        default:
            cout << "[ERROR] Invalid syndrome value.\n";
            break;
    }

    cout << "[DATA] Corrected data bits: ";
    cout << code[2] << " " << code[4] << " " << code[5] << " " << code[6] << "\n";
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int code[8];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "[ERROR] Socket creation failed.\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.56", &serv_addr.sin_addr);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "[ERROR] Bind failed.\n";
        return -1;
    }

    listen(sockfd, 1);
    cout << "[INFO] Receiver listening on 127.0.0.56:8080...\n";

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0) {
        cerr << "[ERROR] Accept failed.\n";
        return -1;
    }

    recv(newsockfd, code, sizeof(code), 0);
    printBits(code, "[RECEIVED] Codeword: ");

    checkAndCorrect(code);

    close(newsockfd);
    close(sockfd);

    return 0;
}
