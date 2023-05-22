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
#include "Setup.h"
#include "Teardown.h"
pthread_mutex_t mutex;


pthread_t sendThread, rcvThread, timerThread;
struct thread_args sendTargs;
int ACK_buffer[MAXSEQ];

// This function is called by the sender thread to send data to the receiver
void sendData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char sendMSG[messageLength];
    
    //pthread_cond_wait(&cond, &mutex); // Wait for the receiver to be ready
    while (runThreads)
    {
        printf("Enter a message to send to the client: ");
        fflush(stdout);
        fgets(sendMSG, messageLength, stdin);
        sendMSG[messageLength - 1] = '\0';
        if(strncmp(sendMSG,"quit\n",messageLength) != 0){
            if (nextSeqNum < base + N)
            {
                pthread_mutex_lock(&mutex); // Lock the mutex
                // Create packet, send it, and store it in the buffer
                sndpkt[nextSeqNum] = make_pkt(nextSeqNum, sendMSG, checksum((u_int8_t*)sendMSG, strlen(sendMSG)));
                udt_send(&sndpkt[nextSeqNum], targs->sockfd, &(targs->addr));
                targs->seqNum = nextSeqNum;
                start_timer(targs,nextSeqNum);
                nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
                printf("Packet sent: %d, Base: %d, Next sequence number: %d\n", sndpkt[nextSeqNum-1].seqNum, base, nextSeqNum);
                pthread_mutex_unlock(&mutex); // Unlock the mutex
            }
        }
        else {
              
            close(targs->sockfd);
            runThreads = false;
            exit(EXIT_SUCCESS);
        }
    }
}

// This function is for ACKs and NACKs from the receiver
void rcvData(void *args)
{
    successMSG("rcvData");
    struct thread_args *targs = (struct thread_args *)args;
    while (runThreads)
    {
        /*if (base >= N) // Reset base if it has reached the end of the buffer
            base = 0;*/

        Packet rcvpkt;
        InitPacket(&rcvpkt);
        rdt_rcv(&rcvpkt, targs->sockfd,&(targs->addr));
        printf("Packet received:\n");
        
        if (rcvpkt.ACK == 1) // If the packet is an ACK
        {
            if (base == rcvpkt.seqNum) // If the ACK is for the packet at the base of the buffer
            {
                stop_timer(rcvpkt.seqNum); // Stop the timer for the packet
                base = (base + 1) % MAXSEQ; // Move the base forward
                successACK(rcvpkt.seqNum);

                // Check for any out of order ACKs
                while (ACK_buffer[base])
                {
                    ACK_buffer[base] = 0;
                    stop_timer(base);
                    printf("Removing out of order ACK for packet sequencenumber %d\n", base);
                    base = (base + 1) % MAXSEQ;
                }
                // TODO: Send FIN packet
            }

            // Buffer out of order ACKs
            else
            {
                successACK(rcvpkt.seqNum);
                printf("Buffering out of order ACK for packet sequencenumber %d\n", rcvpkt.seqNum);
                ACK_buffer[rcvpkt.seqNum] = 1;
                stop_timer(rcvpkt.seqNum);
            }
        }
        else if (rcvpkt.NACK == 1)
        {
            udt_send(&sndpkt[rcvpkt.seqNum], targs->sockfd, &(targs->addr));
            restart_timer(targs,rcvpkt.seqNum); // Restart the timer for the packet
        }
        pthread_mutex_unlock(&mutex); // Unlock the mutex
    }
}



int main()
{
    socklen_t rcvAddrLen;
    base = 1;
    nextSeqNum = 1;

    printf("One small step for dev...\n");

    // Create a UDP socket
    sendTargs.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendTargs.sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("A plethora of bugs to fix!\n");

    // Set up server address
    //memset(&sendTargs.addr, 0, sizeof(sendTargs.addr));
    sendTargs.addr.sin_family = AF_INET;
    sendTargs.addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sendTargs.addr.sin_port = htons(PORT);

    rcvAddrLen = sizeof(sendTargs.addr);

    // Bind the socket to the server address
    if (bind(sendTargs.sockfd, (struct sockaddr *)&(sendTargs.addr), rcvAddrLen) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Bind complete\n");

    ServerSetup(sendTargs.sockfd, (struct sockaddr *)&sendTargs.addr, &rcvAddrLen);

/*
    printf("Connecting to server...\n");
    printf("┌ ・・・・・・・・・・・・・・ ┐\n");
    printf("┊HOST NAME: %s\n", hostName);
    printf("┊IP ADDRESS: %d\n", htonl(sendTargs.addr.sin_addr.s_addr));
    printf("┊PORT: %d\n", ntohs(sendTargs.addr.sin_port));
    printf("┊SOCKET: %d\n", sendTargs.sockfd);
    printf("└ ・・・・・・・・・・・・・・ ┘\n");*/

    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&rcvThread, NULL, (void *)sendData, (void *)&sendTargs);
    
    rcvData(&sendTargs);
    pthread_join(rcvThread, NULL);
    //pthread_create(&sendThread, NULL, (void *)timeout, (void *)&sendTargs);
    //pthread_join(sendThread, NULL);
    

    ServerTeardown(sendTargs.sockfd, (struct sockaddr *)&sendTargs.addr, &rcvAddrLen);

    close(sendTargs.sockfd);
    pthread_mutex_destroy(&mutex);

    return 0;
}
