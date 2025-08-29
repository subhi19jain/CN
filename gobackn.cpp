// sender.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>

#define PORT 8080
#define TOTAL_PACKETS 10
#define WINDOW_SIZE 4
#define BUFFER_SIZE 1024
#define TIMEOUT_MS 2000

using namespace std;

int base = 0, nextSeqNum = 0;
bool timerRunning = false;
chrono::steady_clock::time_point timerStart;

void startTimer() {
    timerRunning = true;
    timerStart = chrono::steady_clock::now();
}

bool isTimeout() {
    if (!timerRunning) return false;
    auto now = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(now - timerStart).count();
    return duration >= TIMEOUT_MS;
}

void resetTimer() {
    timerRunning = false;
}

void sendPacket(int sock, int seqNum) {
    string packet = "SEQ " + to_string(seqNum);
    send(sock, packet.c_str(), packet.size(), 0);
    cout << "[SENDER] Sent packet SEQ " << seqNum << endl;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr{};
    char buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.56", &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed\n";
        return -1;
    }

    cout << "[SENDER] Connected to receiver.\n";

    while (base < TOTAL_PACKETS) {
        // Send packets within the window
        while (nextSeqNum < base + WINDOW_SIZE && nextSeqNum < TOTAL_PACKETS) {
            sendPacket(sock, nextSeqNum);
            if (base == nextSeqNum) startTimer();  // Start timer for the first packet in window
            nextSeqNum++;
        }

        // Wait for ACK (non-blocking)
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;  // 100ms

        int activity = select(sock + 1, &readfds, NULL, NULL, &timeout);

        if (activity > 0 && FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = read(sock, buffer, BUFFER_SIZE);
            if (valread > 0) {
                int ackNum;
                sscanf(buffer, "ACK %d", &ackNum);
                cout << "[SENDER] Received " << buffer << endl;
                if (ackNum >= base) {
                    base = ackNum + 1;
                    if (base == nextSeqNum) {
                        resetTimer();  // No unACKed packets
                    } else {
                        startTimer();  // Restart timer
                    }
                }
            }
        }

        // Timeout: retransmit all packets in window
        if (isTimeout()) {
            cout << "[SENDER] Timeout! Resending window starting at SEQ " << base << endl;
            for (int i = base; i < nextSeqNum; i++) {
                sendPacket(sock, i);
            }
            startTimer();  // Restart timer
        }
    }

    cout << "[SENDER] All packets sent and acknowledged.\n";
    close(sock);
    return 0;
}
