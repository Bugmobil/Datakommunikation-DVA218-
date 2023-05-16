/*
 ============================================================================
 Name        : Server.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : Server (sender) program for the UDP Reliable Transport Protocol
 ============================================================================
 */

#include <semaphore.h>
#include <pthread.h>
#include "udp_transport.h"
#include "Utils.h"

pthread_t sendThread, rcvThread, timerThread;
int ACK_buffer[MAXSEQ];

// This function is called by the sender thread to send data to the receiver
void *sendData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char *sendBuffer = targs->buffer;
    int sockfd = targs->sockfd;
    struct sockaddr_in *dest_addr = targs->addr;
    while (1)
    {
        printf("Enter a message to send to the client: ");
        fgets(sendBuffer, messageLength, stdin);
        sendBuffer[strcspn(sendBuffer, "\n")] = '\0';

        if (nextSeqNum < base + N)
        {
            // Create packet, send it, and store it in the buffer
            sndpkt[nextSeqNum] = make_pkt(nextSeqNum, sendBuffer, checksum(sendBuffer, strlen(sendBuffer)));
            udt_send(&sndpkt[nextSeqNum], sockfd, SERVER_IP);
            sndpkt[nextSeqNum].timestamp = time(NULL); // Store the time the packet was sent to check for timeouts
            nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
        }
    }
}

// This function is for ACKs and NACKs from the receiver
void rcvData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char *rcvBuffer = targs->buffer;
    int sockfd = targs->sockfd;
    struct sockaddr_in *src_addr = targs->addr;
    while (1)
    {
        if (base >= N) // Reset base if it has reached the end of the buffer
            base = 0;

        Packet rcvpkt;
        rdt_rcv(&rcvpkt, sockfd, src_addr);
        if (rcvpkt.ACK == 1) // If the packet is an ACK
        {
            if (base == rcvpkt.seqNum) // If the ACK is for the packet at the base of the buffer
            {
                
                base = (base + 1) % MAXSEQ; // Move the base forward

                // Check for any out of order ACKs
                while (ACK_buffer[base])
                {
                    stop_timer(base);
                    ACK_buffer[base] = 0;
                    base = (base + 1) % MAXSEQ;
                }
            }

            // If the ACK isn't for the packet at the base of the buffer, store it in the buffer
            else if (base < rcvpkt.seqNum)
            {
                ACK_buffer[rcvpkt.seqNum] = 1;
            }
            stop_timer(rcvpkt.seqNum);
        }
        else if (rcvpkt.NACK == 1)
        {
            udt_send(&sndpkt[rcvpkt.seqNum], sockfd, SERVER_IP);
            sndpkt[rcvpkt.seqNum].timestamp = time(NULL);
        }
    }
}

// This function checks if the packets in the buffer have timed out
void timeout(void *args, int seqNum)
{
    struct thread_args *targs = (struct thread_args *)args;
    int sockfd = targs->sockfd;

    while (1)
    {
        if (sndpkt[0].data != NULL) // Start timer after first packet has been sent
            start_timer(seqNum);

        for (int i = 0; i < N; i++)
        {
            int timeLeft = sndpkt[i].timestamp + TIMEOUT - time(NULL);
            if (timeLeft < 0 && sndpkt[i].ACK == 0)
            {
                udt_send(&sndpkt[i], sockfd, SERVER_IP);
                sndpkt[i].timestamp = time(NULL);
            }
        }
        usleep(100000); // Prevent resource hogging
    }
    // TODO: Implement a way to stop the timer if all packets have been ACKed
}

int main()
{
    struct thread_args sendTargs, rcvTargs;
    char sendBuffer[messageLength];
    char rcvBuffer[messageLength];
    base = 1;
    nextSeqNum = 1;

    // Create a UDP socket
    sendTargs.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendTargs.sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&sendTargs.addr, 0, sizeof(sendTargs.addr));
    sendTargs.addr->sin_family = AF_INET;
    sendTargs.addr->sin_addr.s_addr = inet_addr(SERVER_IP);
    sendTargs.addr->sin_port = htons(PORT);

    pthread_create(&sendThread, NULL, sendData, (void *)&sendTargs);
    pthread_create(&rcvThread, NULL, rcvData, (void *)&rcvTargs);
    pthread_create(&timerThread, NULL, timeout, (void *)&sendTargs);
    pthread_join(sendThread, NULL);
    pthread_join(rcvThread, NULL);

    close(sendTargs.sockfd);
    return 0;
}
