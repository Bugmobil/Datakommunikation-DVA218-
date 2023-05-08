/*
This file contains the implementation of the functions declared in udp_transport.h. 
It should include the logic for connection setup and teardown, error check codes, 
sliding window mechanisms, and any other protocol stack functionality.
*/

#include "udp_transport.h"

 
Packet make_pkt(int seqNum, char *data, int checksum)
{
    Packet pkt;
    pkt.seqNum = seqNum;
    strncpy(pkt.data, data, sizeof(pkt.data));
    pkt.checksum = checksum;
    return pkt;
}

void refuse_data(char *data)
{
    printf("Refusing data: %s. Sliding window is full.\n", data);
}

void extract_data(Packet pkt, char *data)
{
    strcpy(data, pkt.data);
}

void deliver_data(char *data)
{
    // TODO: Deliver the received data to the application
}

uint32_t checksum(const uint8_t *data, size_t len) {
    uint32_t hash = 5381;

    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i]; // hash * 33 + data[i]
    }

    return hash;
}



int not_corrupt(Packet pkt)
{
    return pkt.checksum == checksum(pkt.data, strlen(pkt.data));
}

void corrupt(Packet *pkt)
{
    pkt->checksum = 0;
}

void not_duplicate(Packet pkt)
{
    // TODO: Implement functionality to check if the packet is not a duplicate
}

void duplicate(Packet pkt)
{
    // TODO: Implement functionality to handle duplicate packets
}

int has_seq_num(Packet pkt, int expSeqNum)
{
    if (pkt.seqNum == expSeqNum)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void send(char *data)
{
    if (nextSeqNum < base + N)
    {
        sndpkt[nextSeqNum] = make_pkt(nextSeqNum, data, checksum(data, strlen(data)));
        udt_send(&sndpkt[nextSeqNum]);
        start_timer(nextSeqNum);
        nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
    }
    else
    {
        refuse_data(data);
    }
}

void start_timer(int seqNum)
{
    struct itimerval timer;

    // Set the timer interval to 1 second
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // Set the timer to not repeat
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    // Set the signal handler for SIGALRM
    signal(SIGALRM, timeout);

    // Start the timer
    setitimer(ITIMER_REAL, &timer, NULL);

}

void stop_timer(int seqNum)
{
    struct itimerval timer;

    // Clearing timer interval
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;

    // Clearing the interval for repetition
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    // Stop the timer
    setitimer(ITIMER_REAL, &timer, NULL);
}