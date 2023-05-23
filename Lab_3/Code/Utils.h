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
#include <semaphore.h>
#include <pthread.h>

#define PORT 5556
#define SERVER_IP "127.0.0.1"
#define hostNameLength 50
#define messageLength 256
#define PACKET_SIZE FRAMESIZE + sizeof(uint16_t) + 4 * sizeof(uint32_t)
//#define TIMEOUT 1
//#define MAXSEQ 30
//#define N MAXSEQ/2 // window size


/*======== START VALUES ========*/

/*Add this to print on failed run values*/
/*
Error: frame size must be at least 1 byte
Error: link speed must be at least 1 kbps
Error: number of frames must be at least 1
Error: propagation delay must be at least 1 ms
Error: window size must be at least 1

Usage: server <options>

-e ERRORRATE  use ERRORRATE (1-99) as error rate in percent (default 0)
-f FRAMESIZE  use FRAMESIZE (1-3000) as frame size in bytes
-l LINKSPEED  use LINKSPEED (1-10000) in kilobits (1000 bits) per seconds
-n NUMFRAMES  use NUMFRAMES (1-1000) as number of frames
-p PROPDELAY  use PROPDELAY (1-1000) as one-way propagation delay in ms
-t TIMEOUT    use TIMEOUT   (1-5000) as timeout in ms (default 200)
-v            verbose output (shows timing for all frames) 
-w WINSIZE    use WINSIZE   (1-1024) as window size in number of frames
-?            shows this help 
*/

#define ERRORRATE 0 // (1-99) as error rate in percent (default 0)
#define FRAMESIZE 20 // (1-3000) as frame size in bytes
#define LINKSPEED 100 // (1-10000) in kilobits (1000 bits) per seconds
#define NUMFRAMES  30 // (1-1000) as number of frames
#define PROPDELAY  10 // (1-1000) as one-way propagation delay in ms
#define TIMEOUT    20 // (1-5000) as timeout in ms (default 200)
#define WINSIZE    16 // (1-1024) as window size in number of frames

#define TIMEOUTLONG TIMEOUT * (10 + PROPDELAY)
#define TIMEOUTUSEC TIMEOUT * 1000


/*======== END OF START VALUES ========*/

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
   // time_t timestamp;
    uint32_t checksum;
} Packet;

typedef struct
{
    int fd;
    int size;
    int flags;
    char* buffer;
    struct sockaddr *destAddr;
    socklen_t addrLen;
} ThreadSend;


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

//Returns random integer
int GiveRandomNumber(const int from, const int to);
//Randomly corrupts packet
void CorruptPacket(char* packet);
//Randomly corrupts packet based on errorRate. Must be between 0 and 100
void CorruptPacketPercentage(char* packet, int errorRate);

//Sends packet with delay to simulate propagation delay
void ThreadSendDelay(ThreadSend* packet);
//Simulates sending packet with delay, corruption and packet loss
//void SendFaulty(int fd, char* buffer, int size, int flags, struct sockaddr *destAddr, socklen_t addrLen);

// Prints the packet's information
void printPacket(Packet pkt);
void errorLocation(const char *function, const char *file, int line);
void errorMSG(char *msg);
void warningMSG(char *func, char *problem);
void successMSG(char *msg);
void successPKT(int seqNum);
void failMSG(char *msg);
void successACK(int seqNum);
void blueMSG(char *msg);
void execMSG();
#endif