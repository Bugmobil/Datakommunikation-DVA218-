#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdint.h>
#include <netdb.h>


typedef struct {
    int sequenceNumber;
    char* data;
    int dataSize;
    bool ACK;
    time_t timestamp;
    uint32_t checksum;
} Packet;