/*
 ============================================================================
 Name        : udp_transport.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This file contains the implementation of the functions declared in udp_transport.h.
               It should include the logic for connection setup and teardown, error check codes,
               sliding window mechanisms, and any other protocol stack functionality.
 ============================================================================
 */

#include <pthread.h>
#include "udp_transport.h"

/* =============== End of Structs =============== */

/* =============== Globalz =============== */

int base = 0, nextSeqNum = 0, expectedSeqNum = 0;
bool runThreads = true;

// Arrays
Packet sndpkt[NUMFRAMES] = {0};
Packet outOfOrder_buffer[NUMFRAMES] = {0};
int ACK_buffer[NUMFRAMES] = {0};
pthread_t timerThreads[NUMFRAMES] = {0};

/* =============== End of Globalz =============== */

// Stores the packet's relative information
Packet make_pkt(int seqNum, char *data)
{

    Packet pkt;
    InitPacket(&pkt);
    pkt.seqNum = seqNum;
    strncpy(pkt.data, data, sizeof(pkt.data));
    pkt.dataSize = strlen(pkt.data);
    pkt.checksum = checksum(&pkt);

    // printPacket(pkt);
    return pkt;
}

Packet make_ACKpkt(int seqNum, bool ACK, bool NACK)
{
    Packet ACKpkt;
    InitPacket(&ACKpkt);
    ACKpkt.data[0] = '\0';
    ACKpkt.seqNum = seqNum;
    ACKpkt.ACK = ACK;
    ACKpkt.NACK = NACK;
    strcpy(ACKpkt.data, "ACK packet");
    ACKpkt.dataSize = strlen(ACKpkt.data);
    ACKpkt.checksum = checksum(&ACKpkt);

    // printPacket(ACKpkt);
    return ACKpkt;
}
// Sends the packet to the destination address using the UDP socket
void udt_send(Packet *pkt, int sockfd, struct sockaddr_in *dest_addr)
{
    char buffer[PACKET_SIZE];
    Serialize(buffer, *pkt); // Serialize the packet into a buffer

    // Use SendFaulty() to send the serialized packet using the UDP socket
    SendFaulty(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
}

// Receives the packet from the source address using the UDP socket
void rdt_rcv(Packet *pkt, int sockfd, struct sockaddr_in *src_addr)
{
    char buffer[PACKET_SIZE];
    socklen_t addr_len = sizeof(*src_addr);

    // printf(CYN "Ready to receive\n" RESET);

    // Receive data using recvfrom() and store it in the buffer
    int bytes = recvfrom(sockfd, buffer, PACKET_SIZE, 0, src_addr, &addr_len);
    if (bytes < 0)
    {
        errorLocation(__FUNCTION__, __FILE__, __LINE__);
        errorMSG("rdt_rcv --> recvfrom()");
    }
    else
    {
        Deserialize(buffer, pkt); // Deserialize the buffer into a packet
        // successMSG("Deserialize ()");
    }
}

void printLoadingBar()
{
    printf("Loading"); // Start of the loading bar

    for (int i = 0; i < 10; i++)
    {
        int sleepTime = GiveRandomNumber(500000, 1000000); // Generate a random number between 1 and 2
        usleep(sleepTime);                                 // Sleep for the random number of seconds

        printf("█");    // Print a segment of the loading bar
        fflush(stdout); // Flush the output buffer to ensure the segment is printed immediately
    }

    printf("\n"); // End of the loading bar
}

// Extracts the ACK packet and delivers the data
void extractAndDeliver(Packet rcvpkt)
{
    char rcvMsg[messageLength];
    strcpy(rcvMsg, rcvpkt.data);
    //printf("Extracting received data\n");
    //printLoadingBar();
    printf("Received message: %s\n", rcvMsg);
}

// Sends an ACK or NACK packet to the sender
void ACKpkt(struct thread_args *args, bool isACK)
{
    bool ACK = false, NACK = false;
    if (isACK)
        ACK = true;
    else
        NACK = true;

    sndpkt[args->seqNum] = make_ACKpkt(args->seqNum, ACK, NACK);
    udt_send(&sndpkt[args->seqNum], args->sockfd, &(args->addr));
}

/*
Uses the Fletcher's checksum algorithm to calculate
the checksum of the data
*/
uint32_t checksum(Packet *pkt)
{
    uint32_t hash = 5381;

    // Include the boolean fields in the checksum
    hash = ((hash << 5) + hash) + pkt->ACK;
    hash = ((hash << 5) + hash) + pkt->SYN;
    hash = ((hash << 5) + hash) + pkt->FIN;
    hash = ((hash << 5) + hash) + pkt->NACK;

    // Include the data in the checksum
    for (int i = 0; i < pkt->dataSize; i++)
    {
        hash = ((hash << 5) + hash) + pkt->data[i];
    }

    // Include the other fields in the checksum
    hash = ((hash << 5) + hash) + pkt->dataSize;
    hash = ((hash << 5) + hash) + pkt->seqNum;

    return hash;
}

/*
Checks if the packet is corrupted by comparing the received checksum
with the calculated checksum of the received data.
Returns 0 if the packet is not corrupted.
*/
int checkCorrupt(Packet pkt)
{
   
    if (pkt.checksum == checksum(&pkt))
    {
        successMSG("Checksum");
        return 0;
    }
    else
    {
        failMSG("Checksum");
        printf("Expected checksum: %d, Received checksum: %d\n", pkt.checksum, checksum(&pkt));
        return 1;
    }
}

/*
Checks if the received sequence number is the expected sequence number
Returns 0 if the received sequence number is the expected sequence number
*/
int checkSeqNum(int rcvSeqNum, int expSeqNum)
{
    if (rcvSeqNum == expSeqNum)
    {
        successMSG("Sequence number");
        return 0;
    }
    else
    {
        failMSG("Sequence number");
        printf("Expected sequence number: %d, Received sequence number: %d\n", expSeqNum, rcvSeqNum);
        return 1;
    }
}

/*
Timer functions
*/
void start_timer(struct thread_args *args, int seqNum)
{
    //printf("Starting timer for packet %d\n", seqNum);
    pthread_create(&timerThreads[seqNum], NULL, timeout, (void *)args);
}
void stop_timer(int seqNum)
{
    //printf("Stopping timer for packet %d\n", seqNum);
    pthread_cancel(timerThreads[seqNum]);
}
void restart_timer(struct thread_args *args, int seqNum)
{
    //printf("Restarting timer for packet %d\n", seqNum);
    stop_timer(seqNum);
    start_timer(args, seqNum);
}
void *timeout(void *arg)
{
    struct thread_args *targs = (struct thread_args *)arg;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (runThreads)
    {
        // Sleep for the timeout duration
        usleep(TIMEOUTUSEC * 10);
        // Check if the packet has been acknowledged
        if (!sndpkt[targs->seqNum].ACK && !sndpkt[targs->seqNum].NACK)
        {
            printf(RED "Timeout for packet %d\n" RESET, targs->seqNum);
            printf("Retransmitting packet\n");

            // If not, retransmit the packet
            udt_send(&sndpkt[targs->seqNum], targs->sockfd, &(targs->addr));
            restart_timer(targs, targs->seqNum);
        }
    }

    return NULL;
}

void slidingWindow()
{

    printf(YEL "\n\nSliding window " RESET);
    printf("(Size: %d)\n", WINSIZE);
    printf("[ ");
    // sequence numbers
    for (int i = 0; i < WINSIZE; i++)
    {
        if (i == base)
            printf(BLU "%d" RESET, sndpkt[i].seqNum);
        else if (i == nextSeqNum)
            printf(MAG "%d" RESET, sndpkt[i].seqNum);
        else
        printf("%d ", sndpkt[i].seqNum);

        if (i < WINSIZE - 1)
            printf("| ");
    }
    printf("]\n\n");
}
