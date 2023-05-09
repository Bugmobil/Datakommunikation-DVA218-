/*
This file contains the implementation of the functions declared in udp_transport.h. 
It should include the logic for connection setup and teardown, error check codes, 
sliding window mechanisms, and any other protocol stack functionality.
*/

#include "udp_transport.h"
#include "Utils.h"

 
Packet make_pkt(int seqNum, char *data, int checksum)
{
    Packet pkt;
    pkt.seqNum = seqNum;
    strncpy(pkt.data, data, sizeof(pkt.data));
    pkt.checksum = checksum;
    return pkt;
}

/* ============================== SLIDING WINDOW ==========================*/

void udt_send(Packet *pkt, int sockfd, struct sockaddr_in *dest_addr) {
    unsigned char buffer[BUFFER_SIZE];
    size_t buffer_size;

    Serialize(buffer, *pkt);

    // Use sendto() to send the serialized packet using the UDP socket
    int bytes = sendto(sockfd, buffer, buffer_size, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
    if (bytes < 0) {
        perror("Error sending packet");
    }
}

void rdt_rcv(Packet *pkt, int sockfd, struct sockaddr_in *src_addr) {
    unsigned char buffer[BUFFER_SIZE];
    ssize_t recv_size;
    socklen_t addr_len = sizeof(*src_addr);

    // Receive data using recvfrom() and store it in the buffer
    recv_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)src_addr, &addr_len);

    if (recv_size > 0) {
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

void sendData(char* buffer,int sockfd, struct sockaddr_in *dest_addr)
{
    if (nextSeqNum < base + N)
    {
        sndpkt[nextSeqNum] = make_pkt(nextSeqNum, buffer, checksum(buffer, strlen(buffer)));
        udt_send(&sndpkt[nextSeqNum], sockfd, SERVER_IP);
        start_timer(nextSeqNum);
        nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
    }
    else
    {
        refuse_data(buffer);
    }
}

void start_timer(int seqNum)
{
    struct itimerval timer;

    // Set the timer interval to 1 second
    timer.it_value.tv_sec = TIMEOUT;
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