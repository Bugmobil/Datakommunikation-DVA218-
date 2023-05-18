/*
 ============================================================================
 Name        : Client.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : Client (receiver) program for the UDP Reliable Transport Protocol
 ============================================================================
 */

#include <semaphore.h>
#include <pthread.h>
#include "Utils.h"
#include "udp_transport.h"
#include "Setup.h"

pthread_t sendThread, rcvThread;

void dataHandling(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    
    while (1)
    {
        Packet pkt;
        rdt_rcv(&pkt, targs->sockfd, targs->addr);
        if (!checkCorrupt((uint8_t*)pkt.data, pkt.dataSize, pkt.checksum))
        {
            if(!checkSeqNum(pkt.seqNum, expectedSeqNum))
            {
                ACKpkt(targs, true);
                printf(GRN "Expected sequence number received. Sending ACK to server.\n" RESET);
                
                extractAndDeliver(pkt);
                printf(GRN "Packet %d extracted and sent to application layer.\n" RESET, pkt.seqNum);
                
                expectedSeqNum = (expectedSeqNum + 1) % MAXSEQ;
                printf("Expected sequence number incremented to: %d\n", expectedSeqNum);
               
                while (outOfOrder_buffer[expectedSeqNum].seqNum == expectedSeqNum)
                {
                    pkt = outOfOrder_buffer[expectedSeqNum];
                    extractAndDeliver(pkt);
                    printf(YEL "Packet %d extracted from out-of-order buffer and sent to application layer.\n" RESET, pkt.seqNum);
                    expectedSeqNum = (expectedSeqNum + 1) % MAXSEQ;
                    printf("Expected sequence number incremented to: %d\n", expectedSeqNum);
                }
            }
            else
            {
                outOfOrder_buffer[pkt.seqNum] = pkt;
                ACKpkt(targs, true);
                printf(BLU "Packet out of order. Sending ACK to server.\n" RESET);
            }
        }
        else
        {
            printf(RED "Packet is corrupt. Sending NACK to server.\n" RESET);
            ACKpkt(targs, false);
        }
    }
}

void *sendData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char *sendBuffer;
    int sockfd = targs->sockfd;
    struct sockaddr_in *dest_addr = targs->addr;
    while (1)
    {
        printf("Enter a message to send to the server: ");
        fgets(sendBuffer, messageLength, stdin);
        sendBuffer[strcspn(sendBuffer, "\n")] = '\0';

        if (nextSeqNum < base + N)
        {
            sndpkt[nextSeqNum] = make_pkt(nextSeqNum, sendBuffer, checksum((uint8_t*)sendBuffer, strlen(sendBuffer)));
            udt_send(&sndpkt[nextSeqNum], sockfd, dest_addr);
            start_timer(targs, nextSeqNum);
            nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
        }
    }
}

int main(int argc, char *argv[])
{
    char hostName[hostNameLength];    
    struct hostent *hostInfo;
    struct thread_args sendTargs, rcvTargs;
    socklen_t client_addr_len = sizeof(rcvTargs.addr);

    /* Check arguments */
    if(argv[1] == NULL)
    {
        perror("Usage: client [host name]\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        strncpy(hostName, argv[1], hostNameLength);
        hostName[hostNameLength - 1] = '\0';
    }

    hostInfo = gethostbyname(hostName); 

    // Create a UDP socket
    sendTargs.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendTargs.sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    //memset(&sendTargs.addr, 0, sizeof(sendTargs.addr));
    sendTargs.addr->sin_family = AF_INET;
    sendTargs.addr->sin_addr = *(struct in_addr *)hostInfo->h_addr_list[0];
    sendTargs.addr->sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(sendTargs.sockfd, (const struct sockaddr *)&sendTargs.addr, sizeof(sendTargs.addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    //void ClientSetup(int fd, const struct sockaddr* destAddr, socklen_t addrLen);
    ClientSetup(sendTargs.sockfd, (struct sockaddr *)&rcvTargs.addr, &client_addr_len);

    expectedSeqNum = 1;

    pthread_create(&rcvThread, NULL, (void*)dataHandling, (void *)&rcvTargs);
    //pthread_create(&sendThread, NULL, (void*)sendData, (void *)&sendTargs);


    close(sendTargs.sockfd);
    return 0;
}