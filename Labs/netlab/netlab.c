#include <stdio.h>
#include "netsim.h"

int last_received_seq = 0;
int total_packets = 0;
char **packet_data = NULL;      // array of pointers to data
int *packet_lens = NULL;        // array of lengths
int pending_timeout = -1;       // to clear/restart timeout
char requested_msg_number = '0';  // set from argv[1]

void send_GET(void *arg) {
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = requested_msg_number;
    data[0] = checksum(5, data);
    send(5, data);
    pending_timeout = setTimeout(send_GET, 1000, NULL);
}

void send_ACK(unsigned char seq) {
    char data[5];
    data[1] = 'A'; data[2] = 'C'; data[3] = 'K'; data[4] = seq;
    data[0] = checksum(5, data);
    send(5, data);
}

void resend_last_ACK(void *arg) {
    if (last_received_seq == 0)
        send_GET(NULL);
    else
        send_ACK(last_received_seq);
    pending_timeout = setTimeout(resend_last_ACK, 1000, NULL);
}

int checksum(int len, char *data) {
    int xor = data[1];
    for (int i = 2; i < len; i++) {
        xor ^= data[i];
    }
    return xor;
}

void recvd(size_t len, void* _data) {
    // FIX ME -- add proper handling of messages
    char *data = _data;
    if (checksum(len, data) != data[0]) return; // drop bad packet

    unsigned char seq = data[1];
    unsigned char total = data[2];

    if (seq == 1 && last_received_seq == 0) {
        // first packet
        total_packets = total;
        packet_data = calloc(total + 1, sizeof(char*));
        packet_lens = calloc(total + 1, sizeof(int));
    }

    if (seq > total_packets || seq < 1) return;

    // Save if new
    if (!packet_data[seq]) {
        packet_data[seq] = malloc(len - 3);
        memcpy(packet_data[seq], data + 3, len - 3);
        packet_lens[seq] = len - 3;
    }

    if (seq == last_received_seq + 1) {
        // Write contiguous data
        while (packet_data[last_received_seq + 1]) {
            last_received_seq++;
            fwrite(packet_data[last_received_seq], 1, packet_lens[last_received_seq], stdout);
            fflush(stdout);
        }
    }

    // ACK and restart timeout
    if (pending_timeout > 0) clearTimeout(pending_timeout);
    send_ACK(last_received_seq);
    pending_timeout = setTimeout(resend_last_ACK, 1000, NULL);
}

int main(int argc, char *argv[]) {
    // this code should work without modification
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }
    
    requested_msg_number = argv[1][0];
    send_GET(NULL);

    waitForAllTimeoutsAndMessagesThenExit();
}
