#include <stdio.h>
#include "netsim.h"

void send_GET() {
    
}

void send_ACK() {
    
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
    fwrite(data+3,1,len-3,stdout);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    // this code should work without modification
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = argv[1][0];
    // end of working code
    
    
    data[0] = checksum(5, data); // FIX ME -- add proper checksum computation
    send(5, data);
    // FIX ME -- add action if no reply

    waitForAllTimeoutsAndMessagesThenExit();
}
