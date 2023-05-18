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
struct thread_args sendTargs, rcvTargs;
int ACK_buffer[MAXSEQ];

// This function is called by the sender thread to send data to the receiver
void sendData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char *sendMSG[messageLength];
    printf("Enter a message to send to the client: ");
    while (runThreads)
    {
        printf("\n");
        fgets(*sendMSG, messageLength, stdin);
        sendMSG[messageLength - 1] = '\0';
        if(strncmp(*sendMSG,"quit\n",messageLength) != 0){
            if (nextSeqNum < base + N)
            {
                // Create packet, send it, and store it in the buffer
                sndpkt[nextSeqNum] = make_pkt(nextSeqNum, *sendMSG, checksum((u_int8_t*)sendMSG, strlen(*sendMSG)));
                udt_send(&sndpkt[nextSeqNum], targs->sockfd, targs->addr);
                targs->seqNum = nextSeqNum;
                start_timer(targs,nextSeqNum);
                nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
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
    struct thread_args *targs = (struct thread_args *)args;
    while (runThreads)
    {
        /*if (base >= N) // Reset base if it has reached the end of the buffer
            base = 0;*/

        Packet rcvpkt;
        rdt_rcv(&rcvpkt, targs->sockfd, targs->addr);
        if (rcvpkt.ACK == 1) // If the packet is an ACK
        {
            if (base == rcvpkt.seqNum) // If the ACK is for the packet at the base of the buffer
            {
                stop_timer(rcvpkt.seqNum); // Stop the timer for the packet
                base = (base + 1) % MAXSEQ; // Move the base forward

                // Check for any out of order ACKs
                while (ACK_buffer[base])
                {
                    ACK_buffer[base] = 0;
                    stop_timer(base);
                    base = (base + 1) % MAXSEQ;
                }
            }

            // Buffer out of order ACKs
            else
            {
                ACK_buffer[rcvpkt.seqNum] = 1;
                stop_timer(rcvpkt.seqNum);
            }
        }
        else if (rcvpkt.NACK == 1)
        {
            udt_send(&sndpkt[rcvpkt.seqNum], targs->sockfd, targs->addr);
            restart_timer(targs,rcvpkt.seqNum); // Restart the timer for the packet
        }
    }
}



int main()
{
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
    sendTargs.addr->sin_family = AF_INET;
    sendTargs.addr->sin_addr.s_addr = INADDR_ANY;
    sendTargs.addr->sin_port = htons(PORT);

    printf("So far so good\n");

    pthread_create(&sendThread, NULL, (void *)sendData, (void *)&sendTargs);
    pthread_create(&rcvThread, NULL, (void *)rcvData, (void *)&rcvTargs);
    pthread_join(sendThread, NULL);
    pthread_join(rcvThread, NULL);

    close(sendTargs.sockfd);
    return 0;
}
