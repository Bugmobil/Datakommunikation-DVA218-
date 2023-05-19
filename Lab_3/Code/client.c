/*
 ============================================================================
 Name        : Client.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : Client (receiver) program for the UDP Reliable Transport Protocol
 ============================================================================
 */

#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include "udp_transport.h"
#include "Setup.h"
#include "Teardown.h"

pthread_t sendThread, rcvThread;
pthread_mutex_t mutex;


void dataHandling(void *args)
{
    printf("Initializing data handling thread.\n");
    struct thread_args *targs = (struct thread_args *)args;
    
    while (runThreads)
    {
        Packet pkt;
        InitPacket(&pkt);
        rdt_rcv(&pkt, targs->sockfd, &(targs->addr));
        pkt.dataSize = strlen(pkt.data);
        if (pkt.dataSize != 0)
        {
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
}

/*void *sendData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char *sendBuffer[messageLength];
    int sockfd = targs->sockfd;
    struct sockaddr_in *dest_addr = &(targs->addr);
    while (1)
    {
        printf("Enter a message to send to the server: ");
        fgets(*sendBuffer, messageLength, stdin);
        sendBuffer[strcspn(*sendBuffer, "\n")] = '\0';

        if (nextSeqNum < base + N)
        {
            sndpkt[nextSeqNum] = make_pkt(nextSeqNum, *sendBuffer, checksum((uint8_t*)sendBuffer, strlen(*sendBuffer)));
            udt_send(&sndpkt[nextSeqNum], sockfd, dest_addr);
            start_timer(targs, nextSeqNum);
            nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
        }
    }
}*/

void initSocketAddress(struct sockaddr_in *name, char *hostName, unsigned short int port)
{
    name->sin_family = AF_INET;  
    name->sin_port = htons(port); 
    struct hostent *hostInfo = gethostbyname(hostName); 

    if(hostInfo == NULL)
    {
        errorLocation(__FUNCTION__,__FILE__, __LINE__);
        perror("Unknown host: \n");
        exit(EXIT_FAILURE);
    }
    printf("Host name: %s\n", hostInfo->h_name);
    name->sin_addr = *(struct in_addr *)hostInfo->h_addr_list[0];

}

int main(int argc, char *argv[])
{
    char hostName[hostNameLength];
  //  struct hostent *hostInfo;
    struct thread_args sendTargs;
    socklen_t serverAddrLen;

    /* Check arguments */
    if(argv[1] == NULL)
    {
        errorLocation(__FUNCTION__,__FILE__, __LINE__);
        perror("Usage: client [host name]\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        strncpy(hostName, argv[1], hostNameLength);
        hostName[hostNameLength - 1] = '\0';
    }

    //hostName[hostNameLength - 1] = '\0';

    // Create a UDP socket
    sendTargs.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sendTargs.sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    initSocketAddress(&(sendTargs.addr), hostName, PORT);
    serverAddrLen = sizeof(sendTargs.addr);
    ClientSetup(sendTargs.sockfd, (struct sockaddr *)&(sendTargs.addr), &serverAddrLen);

    int flags = fcntl(sendTargs.sockfd, F_GETFL, 0);
    if (flags & O_NONBLOCK) {
        printf("Socket is non-blocking\n");
    } else {
        printf("Socket is blocking\n");
    }

    printf("Connecting to server...\n");
    printf("┌ ・・・・・・・・・・・・・・ ┐\n");
    printf("┊HOST NAME: %s\n", hostName);
    printf("┊IP ADDRESS: %s\n", inet_ntoa(sendTargs.addr.sin_addr));
    printf("┊PORT: %d\n", ntohs(sendTargs.addr.sin_port));
    printf("┊SOCKET: %d\n", sendTargs.sockfd);
    printf("└ ・・・・・・・・・・・・・・ ┘\n");

    expectedSeqNum = 1;
    dataHandling((void *)&sendTargs);
    //pthread_create(&rcvThread, NULL, (void*)dataHandling, (void *)&sendTargs);
    //pthread_join(rcvThread, NULL);
    //pthread_create(&sendThread, NULL, (void*)sendData, (void *)&sendTargs);


    ClientTeardown(sendTargs.sockfd, (struct sockaddr *)&(sendTargs.addr), &serverAddrLen);    
    close(sendTargs.sockfd);
    return 0;
}