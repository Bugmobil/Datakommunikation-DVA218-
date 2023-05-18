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

const char *testData[24] = {
    "This", "is", "the" ,"frames" ,"that" ,"we" ,
    "want" ,"to", "send" ,"to", "the", "server",
    "in", "correct", "order", "and", "without", 
    "errors", "in", "the", "data", "or", "the", "ACKs"
    };

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

/* Error Handling */
void errorMSG(char *msg)
{
    printf(RED "Error in function: %s\n" RESET,msg);
    exit(EXIT_FAILURE);
}

void warningMSG(char func, char *problem)
{
    printf(YEL "Warning in function: %s"RESET, func);
    printf("Problem: %s\n", problem);
}

void successMSG(char *msg)
{
    printf("Function %s executed", msg);
    printf(GRN " successfully\n" RESET);
}

void corruptedMSG(int seqNum)
{
    printf(RED "Corrupted packet! " RESET);
    printf("Sequence number: ");
    printf(MAG " %d\n" RESET, seqNum);
}
/* End of Error Handling */

// Prints the packet's information
void printPacket (Packet pkt)
{
    printf("┌ ・・・・・・・・・・・・・・ ┐");
    printf("┊DATA: %s\n", pkt.data);
    printf("┊SEQ NUM: %d\n", pkt.seqNum);
    printf("┊ACK/NACK: %d/%d\n", pkt.ACK, pkt.NACK);
    printf("┊CHECKSUM: %d\n", pkt.checksum);
    printf("└ ・・・・・・・・・・・・・・ ┘");
}
void InitPacket(Packet *packet);
void Serialize(char *buffer, Packet packet);
void Deserialize(char *buffer, Packet *packet);

void StartTimer(time_t* startTime);
int CheckTime(time_t startTime, int timeout);

int GiveRandomNumber(int from, int to);

void CorruptPacket(char* packet);
void CorruptPacketPercentage(char* packet, int errorRate);

#endif