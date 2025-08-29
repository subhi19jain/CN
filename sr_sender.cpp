#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <sys/select.h>
#include <vector>

#define PORT 8080
#define TOTAL_PACKETS 8
#define WINDOW_SIZE 4
#define TIMEOUT_MS 3000
#define BUFFER_SIZE 1024

using namespace std;

struct Packet {
    int seqNum;
    bool acked = false;
    chrono::steady_clock::time_point sendTime;
};

void sendPacket(int sock, int seqNum) {
    string msg = "SEQ " + to_string(seqNum);
    send(sock, msg.c_str(), msg.length(), 0);
    cout << "[SENDER] Sent SEQ " << seqNum << endl;
}

bool isTimeout(Packet &pkt) {
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - pkt.sendTime).count();
    return elapsed >= TIMEOUT_MS;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr{};
    char buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.56", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "[SENDER] Connection Failed\n";
        return -1;
    }

    cout << "[SENDER] Connected to receiver.\n";

    vector<Packet> packets(TOTAL_PACKETS);
    for (int i = 0; i < TOTAL_PACKETS; i++) {
        packets[i].seqNum = i;
    }

    int base = 0;

    while (base < TOTAL_PACKETS) {
        // Send packets in the window
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_PACKETS; i++) {
            if (!packets[i].acked && packets[i].sendTime.time_since_epoch().count() == 0) {
                sendPacket(sock, i);
                packets[i].sendTime = chrono::steady_clock::now();
            }
        }

        // Check for ACK
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
                cout << "[SENDER] Received ACK " << ackNum << endl;
                packets[ackNum].acked = true;

                // Slide window
                while (base < TOTAL_PACKETS && packets[base].acked) {
                    base++;
                }
            }
        }

        // Handle timeouts
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_PACKETS; i++) {
            if (!packets[i].acked && isTimeout(packets[i])) {
                cout << "[SENDER] Timeout for SEQ " << i << ", resending...\n";
                sendPacket(sock, i);
                packets[i].sendTime = chrono::steady_clock::now();
            }
        }
    }

    cout << "[SENDER] All packets sent and acknowledged.\n";
    close(sock);
    return 0;
}
