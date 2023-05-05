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

#define PORT 5555
#define hostNameLength 50
#define messageLength 256
#define MAXMSG 512

typedef struct {
    int sequenceNumber;
    char data[MAXMSG];
    int dataSize;
    bool ACK;
    bool SYN;
    bool FIN;
    time_t timestamp;
    uint32_t checksum;
} Packet;