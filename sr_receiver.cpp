#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

#define PORT 8080
#define TOTAL_PACKETS 8
#define WINDOW_SIZE 4
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.56");
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    cout << "[RECEIVER] Waiting on 127.0.0.56:8080...\n";

    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    cout << "[RECEIVER] Connected to sender.\n";

    vector<bool> received(TOTAL_PACKETS, false);
    int expected_base = 0;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes <= 0) break;

        int recv_seq;
        sscanf(buffer, "SEQ %d", &recv_seq);

        cout << "[RECEIVER] Received SEQ " << recv_seq << endl;

        if (recv_seq >= expected_base && recv_seq < expected_base + WINDOW_SIZE) {
            if (!received[recv_seq]) {
                received[recv_seq] = true;
                cout << "[RECEIVER] Stored SEQ " << recv_seq << endl;
            }

            string ack = "ACK " + to_string(recv_seq);
            send(new_socket, ack.c_str(), ack.length(), 0);
            cout << "[RECEIVER] Sent ACK " << recv_seq << endl;

            // Slide window if possible
            while (expected_base < TOTAL_PACKETS && received[expected_base]) {
                expected_base++;
            }

            if (expected_base == TOTAL_PACKETS) {
                cout << "[RECEIVER] All packets received.\n";
                break;
            }
        } else {
            // Out of window: resend ACK if already received
            if (recv_seq < TOTAL_PACKETS && received[recv_seq]) {
                string ack = "ACK " + to_string(recv_seq);
                send(new_socket, ack.c_str(), ack.length(), 0);
                cout << "[RECEIVER] Re-sent ACK " << recv_seq << endl;
            }
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
