/*
This header file contains declarations of any utility functions or 
data structures you might need for your implementation, 
such as error handling, logging, or conversion functions.
*/
#ifndef UTILS_H
#define UTILS_H

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

#define PORT 5555
#define hostNameLength 50
#define messageLength 256
#define MAXMSG messageLength + sizeof(uint16_t) + 4 * sizeof(uint32_t)

void InitPacket(Packet* packet);
void Serialize(char* buffer, Packet packet);
void Deserialize(char* buffer, Packet* packet)

typedef struct {
    bool ACK;
    bool SYN;
    bool FIN;
    char data[messageLength];
    int dataSize;
    int seqNum;
    time_t timestamp;
    uint32_t checksum;
} Packet;


#endif