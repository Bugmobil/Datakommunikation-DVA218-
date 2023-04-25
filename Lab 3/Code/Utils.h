#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdint.h>


typedef struct {
    int sequenceNumber;
    char* data;
    int dataSize;
    bool ACK;
    time_t timestamp;
    unit32_t checksum;
} Packet;