/*
This header file contains declarations of the main data structures and functions related to our protocol stack implementation. 
It should define structures for packets, sliding window, and any other relevant data structures. 
It should also declare functions for connection setup and teardown, 
error check codes, and sliding window mechanisms.
*/

#ifndef UDP_TRANSPORT_H
#define UDP_TRANSPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <netinet/in.h>
#include <Utils.h>

#define N 5 // window size
#define MAXSEQ 25600

int base, nextSeqNum;

typedef struct {
    int base;
    int nextSeqNum;
    int windowSize;
} CircularBuffer;



#endif