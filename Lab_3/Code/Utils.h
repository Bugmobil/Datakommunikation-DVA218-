/*
 ============================================================================
 Name        : Utils.h
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This header file contains declarations of any utility functions or
                data structures you might need for your implementation,
                such as error handling, logging, or conversion functions.
 ============================================================================
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdint.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>

#define PORT 5555
#define SERVER_IP "127.0.0.1"
#define hostNameLength 50
#define messageLength 256
#define PACKET_SIZE messageLength + sizeof(uint16_t) + 4 * sizeof(uint32_t)
#define TIMEOUT 1
#define TIMEOUTLONG 5
#define N 5 // window size
#define MAXSEQ 25600
#define MAX_PKT 10

char hostName[hostNameLength];



typedef struct
{
    bool ACK;
    bool SYN;
    bool FIN;
    bool NACK;
    char data[messageLength];
    int dataSize;
    int seqNum;
    time_t timestamp;
    uint32_t checksum;
} Packet;

/* Terminal Colors */

/*Example: printf(RED "This text will be red\n" RESET); */
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

/* End of Terminal Colors */

void InitPacket(Packet *packet);
void Serialize(char *buffer, Packet packet);
void Deserialize(char *buffer, Packet *packet);

void StartTimer(time_t* startTime);
int CheckTime(time_t startTime, int timeout);

int GiveRandomNumber(int from, int to);

void CorruptPacket(char* packet);
void CorruptPacketPercentage(char* packet, int errorRate);

#endif