/*
 ============================================================================
 Name        : udp_transport.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This file contains the implementation of the functions declared in udp_transport.h.
               It should include the logic for connection setup and teardown, error check codes,
               sliding window mechanisms, and any other protocol stack functionality.
 ============================================================================
 */

#include "udp_transport.h"
#include "Utils.h"

// Stores the packet's relative information
Packet make_pkt(int seqNum, char *data, int checksum)
{
    Packet pkt;
    pkt.seqNum = seqNum;
    strncpy(pkt.data, data, sizeof(pkt.data));
    pkt.checksum = checksum;
    
    printPacket(pkt);
    return pkt;
}

Packet make_ACKpkt(int seqNum, bool ACK, bool NACK)
{
    Packet ACKpkt;
    ACKpkt.seqNum = seqNum;
    ACKpkt.ACK = ACK;
    ACKpkt.NACK = NACK;
    ACKpkt.checksum = checksum((uint8_t *)&ACKpkt, sizeof(ACKpkt));

    printPacket(ACKpkt);
    return ACKpkt;
}

void extractAndACK(Packet ACKpkt, struct thread_args *args, bool isACK)
{
    bool ACK = false, NACK = false;
    if(isACK)
        ACK = true;
    else
        NACK = true;

    extract_data(ACKpkt, args->buffer);
    deliver_data(args->buffer);
    sndpkt[expectedSeqNum] = make_ACKpkt(expectedSeqNum, ACK, NACK);
    udt_send(&sndpkt[expectedSeqNum], args->sockfd, args->addr);
}
// Sends the packet to the destination address using the UDP socket
void udt_send(Packet *pkt, int sockfd, struct sockaddr_in *dest_addr)
{
    char buffer[PACKET_SIZE];

    Serialize(buffer, *pkt); // Serialize the packet into a buffer

    // Use sendto() to send the serialized packet using the UDP socket
    int bytes = sendto(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
    if (bytes < 0)
    {
        perror("Error sending packet");
    }
}

// Receives the packet from the source address using the UDP socket
void rdt_rcv(Packet *pkt, int sockfd, struct sockaddr_in *src_addr)
{
    char buffer[PACKET_SIZE];
    ssize_t recv_size;
    socklen_t addr_len = sizeof(*src_addr);

    // Receive data using recvfrom() and store it in the buffer
    recv_size = recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)src_addr, &addr_len);

    if (recv_size > 0)
    {
        // Deserialize the received buffer and populate the packet structure
        Deserialize(buffer, pkt);
    }
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

/*
Uses the Fletcher's checksum algorithm to calculate
the checksum of the data 
*/
uint32_t checksum(const uint8_t *data, size_t len)
{
    uint32_t hash = 5381;

    for (size_t i = 0; i < len; i++)
    {
        hash = ((hash << 5) + hash) + data[i]; // hash * 33 + data[i]
    }

    return hash;
}

/*
Checks if the packet is corrupted by comparing the received checksum
with the calculated checksum of the received data.
Returns 0 if the packet is not corrupted. 
*/
int checkCorrupt(const uint8_t *data, size_t len, uint32_t rcvChecksum)
{
    return rcvChecksum - checksum(data, len);
}

/*
Checks if the received sequence number is the expected sequence number
Returns 0 if the received sequence number is the expected sequence number 
*/
int checkSeqNum(int rcvSeqNum, int expSeqNum)
{
    return rcvSeqNum - expSeqNum;
}

void start_timer(struct thread_args *args, int seqNum)
{
    pthread_create(&timerThreads[seqNum], NULL, timeout, (void *)args);
}
void stop_timer(int seqNum)
{
    pthread_cancel(timerThreads[seqNum]);
}
void restart_timer(struct thread_args *args, int seqNum)
{
    stop_timer(seqNum);
    start_timer(args, seqNum);
}
void *timeout(void *arg) {
    struct thread_args *targs = (struct thread_args *)arg;
    
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (runThreads) {
        // Sleep for the timeout duration
        sleep(TIMEOUT);
        // Check if the packet has been acknowledged
        if (!sndpkt[targs->seqNum].ACK && !sndpkt[targs->seqNum].NACK) {
            // If not, retransmit the packet
            udt_send(&sndpkt[targs->seqNum], targs->sockfd, targs->addr);
        }
    }
    
    return NULL;
}

// This function checks if the packets in the buffer have timed out
/*void timeout(void *args, int seqNum)
{
    struct thread_args *targs = (struct thread_args *)args;
    int sockfd = targs->sockfd;
    char *dest_addr = targs->addr;

    while (1)
    {
        for (int i = 0; i < N; i++)
        {
            int timeLeft = sndpkt[i].timestamp + TIMEOUT - time(NULL);
            if (timeLeft < 0 && sndpkt[i].ACK == 0)
            {
                udt_send(&sndpkt[i], sockfd, dest_addr);
                StartTimer(sndpkt[nextSeqNum].timestamp);
            }
        }
        usleep(100000); // Prevent resource hogging
        if(nextSeqNum == base)
            break;
    }
    // TODO: Implement a way to stop the timer if all packets have been ACKed
}
*/
/* Removes the packet from the buffer and hence stops the timer */
/*void stop_timer(int seqNum)
{
    for (int i = 0; i < MAX_PKT; i++)
    {
        if (sndpkt[i].seqNum == seqNum)
        {
            sndpkt[i] = make_pkt(NULL, NULL, NULL);
            break;
        }
    }
}*/


void printPacket (Packet pkt)
{
    printf("┌ ・・・・・・・・・・・・・・ ┐");
    printf("┊DATA: %s\n", pkt.data);
    printf("┊SEQ NUM: %d\n", pkt.seqNum);
    printf("┊ACK/NACK: %d/%d\n", pkt.ACK, pkt.NACK);
    printf("┊CHECKSUM: %d\n", pkt.checksum);
    printf("└ ・・・・・・・・・・・・・・ ┘");
}