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

#define PORT 5556
#define SERVER_IP "127.0.0.1"
#define hostNameLength 50
#define messageLength 256
#define PACKET_SIZE messageLength + sizeof(uint16_t) + 4 * sizeof(uint32_t)
#define TIMEOUT 1
#define TIMEOUTLONG TIMEOUT * 10
#define TIMEOUTUSEC TIMEOUT * 1000
#define N 5 // window size
#define MAXSEQ 25600
#define MAX_PKT 10

char hostName[hostNameLength];
extern char *testData;

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

// Initializes packet by setting values to zero
void InitPacket(Packet *packet);
// Converts packet into a format sendto() can send
void Serialize(char *buffer, Packet packet);
// Converts serialized packet into packet struct
void Deserialize(char *buffer, Packet *packet);

// Sends the flags specified by the flags argument. Argument flags must be four single bit numbers. Order goes ACK -> SYN -> FIN -> NACK
void SendFlagPacket(int fd, struct sockaddr *destAddr, socklen_t addrLen, const char* flags);
// Receives the flags specified by the flags argument. Argument flags must be four single bit numbers. Order goes ACK -> SYN -> FIN -> NACK
int ReceiveFlagPacket(int fd, struct sockaddr *src_addr, socklen_t *addrLen, const char* flags);

//Sets timer to current time
void StartTimer(struct timeval* startTime);
//Returns 1 if timeout has surpassed starTime. Otherwise returns 0
int CheckTime(struct timeval startTime, int timeout);

//Return random integer
int GiveRandomNumber(const int from, const int to);

//Randomly corrupts packet
void CorruptPacket(char* packet);
//Randomly corrupts packet based on errorRate. Must be between 0 and 100
void CorruptPacketPercentage(char* packet, int errorRate);

// Prints the packet's information
void printPacket(Packet pkt);
void errorLocation(char *function, char *file, int line);
void errorMSG(char *msg);
void warningMSG(char *func, char *problem);
void successMSG(char *msg);
void corruptedMSG(int seqNum);

#endif