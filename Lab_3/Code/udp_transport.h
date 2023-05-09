/*
This header file contains declarations of the main data structures and functions related to our protocol stack implementation. 
It should define structures for packets, sliding window, and any other relevant data structures. 
It should also declare functions for connection setup and teardown, 
error check codes, and sliding window mechanisms.
*/

#ifndef UDP_TRANSPORT_H
#define UDP_TRANSPORT_H

#include "Utils.h"


/* =============== Globalz =============== */

int base = 0, nextSeqNum = 0, expectedSeqNum;


Packet sndpkt[MAX_PKT];

/* =============== Functions =============== */

// Functions for packet handling
Packet make_pkt(int seqNum, char* data, int checksum);
void udt_send(Packet *pkt, int sockfd, struct sockaddr_in *dest_addr);
void rdt_rcv(Packet *pkt, int sockfd, struct sockaddr_in *src_addr);
void refuse_data(char* data);
void extract_data(Packet pkt, char* data);
void deliver_data(char* data);

// Functions for packet validation
uint32_t checksum(const uint8_t *data, size_t len);
int not_corrupt(Packet pkt);
void corrupt(Packet *pkt);
void not_duplicate(Packet pkt);
void duplicate(Packet pkt);
int has_seq_num(Packet pkt, int seqNum);

// Send function
void sendData(char* buffer,int sockfd, struct sockaddr_in *dest_addr);

// Functions for timers
void timeout(int seqNum);
void start_timer(int seqNum);
void stop_timer(int seqNum);

#endif