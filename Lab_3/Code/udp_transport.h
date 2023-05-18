/*
 ============================================================================
 Name        : udp_transport.h
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This header file contains declarations of the main data structures and functions related to our protocol stack implementation.
               It should define structures for packets, sliding window, and any other relevant data structures.
               It should also declare functions for connection setup and teardown,
               error check codes, and sliding window mechanisms.
 ============================================================================
 */

#ifndef UDP_TRANSPORT_H
#define UDP_TRANSPORT_H

#include "Utils.h"

/* =============== Structs =============== */

struct thread_args
{
    int seqNum;
    int sockfd;
    struct sockaddr_in addr;
};

/* =============== End of Structs =============== */

/* =============== Globalz =============== */

extern int base, nextSeqNum, expectedSeqNum;
extern bool runThreads;

// Arrays
extern Packet sndpkt[MAX_PKT];
extern Packet outOfOrder_buffer[MAXSEQ];
extern int ACK_buffer[MAXSEQ];
extern pthread_t timerThreads[MAXSEQ];

/* =============== End of Globalz =============== */

/* =============== Functions =============== */

// Most nessessary function

void printPacket(Packet pkt);

// Functions for packet handling
Packet make_pkt(int seqNum, char *data, int checksum);
Packet make_ACKpkt(int seqNum, bool ACK, bool NACK);
void extractAndDeliver(Packet rcvpkt);

void udt_send(Packet *pkt, int sockfd, struct sockaddr_in *dest_addr);
void rdt_rcv(Packet *pkt, int sockfd, struct sockaddr_in *src_addr);
void ACKpkt(struct thread_args *args, bool isACK);

// Functions for packet validation
uint32_t checksum(const uint8_t *data, size_t len);
int checkCorrupt(const uint8_t *data, size_t len, uint32_t rcvChecksum);
int checkSeqNum(int rcvSeqNum, int expSeqNum);

// Functions for timers
void start_timer(struct thread_args *args, int seqNum);
void restart_timer(struct thread_args *args, int seqNum);
void stop_timer(int seqNum);
void *timeout(void *arg);


/* =============== End of Functions =============== */

#endif