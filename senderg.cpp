// sender.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <sys/select.h>

#define PORT 8080
#define TOTAL_PACKETS 8      // Updated to 8
#define WINDOW_SIZE 4        // Updated to 4
#define TIMEOUT_MS 3000
#define BUFFER_SIZE 1024

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
    string msg = "SEQ " + to_string(seqNum);
    send(sock, msg.c_str(), msg.length(), 0);
    cout << "[SENDER] Sent SEQ " << seqNum << endl;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr{};
    char buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.56", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "[SENDER] Connection Failed\n";
        return -1;
    }

    cout << "[SENDER] Connected to receiver.\n";

    while (base < TOTAL_PACKETS) {
        while (nextSeqNum < base + WINDOW_SIZE && nextSeqNum < TOTAL_PACKETS) {
            sendPacket(sock, nextSeqNum);
            if (base == nextSeqNum) startTimer();
            nextSeqNum++;
            this_thread::sleep_for(chrono::milliseconds(200));  // Small delay between sends
        }

        // Check for ACK using select
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        struct timeval timeout = {0, 100000}; // 100 ms

        int activity = select(sock + 1, &readfds, NULL, NULL, &timeout);

        if (activity > 0 && FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            int len = read(sock, buffer, BUFFER_SIZE);
            if (len > 0) {
                int ackNum;
                sscanf(buffer, "ACK %d", &ackNum);
                cout << "[SENDER] Received " << buffer << endl;
                if (ackNum >= base) {
                    base = ackNum + 1;
                    if (base == nextSeqNum) resetTimer();
                    else startTimer();
                }
            }
        }

        if (isTimeout()) {
            cout << "[SENDER] Timeout. Resending window from SEQ " << base << endl;
            for (int i = base; i < nextSeqNum; i++) {
                sendPacket(sock, i);
                this_thread::sleep_for(chrono::milliseconds(100));
            }
            startTimer();
        }
    }

    cout << "[SENDER] All packets sent and ACKed.\n";
    close(sock);
    return 0;
}
