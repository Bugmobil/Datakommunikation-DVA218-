/*
 ============================================================================
 Name        : Server.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : Server (sender) program for the UDP Reliable Transport Protocol
 ============================================================================
 */

#include "udp_transport.h"
#include "Utils.h"
#include "Setup.h"
#include "Teardown.h"
pthread_mutex_t mutex;

pthread_t rcvThread, sendThread, timerThread;
struct thread_args sendTargs;
int ACK_buffer[NUMFRAMES];
int ackCount = 0;
time_t start, end;
int framesSent = 0;

void GenerateMGS(char *sendMSG, int maxLen)
{
    size_t i;
    int length = GiveRandomNumber(5, maxLen - 1);
    for (i = 0; i < length; i++)
    {
        sendMSG[i] = GiveRandomNumber(65, 90);
    }
    sendMSG[i + 1] = '\0';
}

// This function is called by the sender thread to send data to the receiver
void sendData(void *args)
{
    struct thread_args *targs = (struct thread_args *)args;
    char sendMSG[FRAMESIZE];
    start = time(NULL);
    execMSG();
    sleep(1);
    while (runThreads)
    {

        sleep(1);
        GenerateMGS(sendMSG, FRAMESIZE);
        if (framesSent != NUMFRAMES)
        {
            if (nextSeqNum < base + WINSIZE)
            {
                pthread_mutex_lock(&mutex); // Lock the mutex
                // Create packet, send it, and store it in the buffer
                sndpkt[nextSeqNum] = make_pkt(nextSeqNum, sendMSG, framesSent);
                udt_send(&sndpkt[nextSeqNum], targs->sockfd, &(targs->addr));
                targs->seqNum = nextSeqNum;
                start_timer(targs, nextSeqNum);

                printf("Sending packet: %d", framesSent);
                printPacket(sndpkt[nextSeqNum]);

                nextSeqNum = (nextSeqNum + 1) % WINSIZE;
                framesSent++;
                pthread_mutex_unlock(&mutex); // Unlock the mutex

                slidingWindow();
                fflush(stdout);
            }
        }
        else
        {
            successMSG("All packets sent. Waiting for ACKs.");
            fflush(stdout);
            pthread_exit(NULL);
        }
    }
}

// This function is for ACKs and NACKs from the receiver
void rcvData(void *args)
{
    // successMSG("rcvData");
    struct thread_args *targs = (struct thread_args *)args;
    while (runThreads)
    {
        Packet rcvpkt;
        InitPacket(&rcvpkt);
        if (ackCount == NUMFRAMES)
        {
            printf("ACKs received: %d\n", ackCount);
            fflush(stdout);

            Packet pkt = make_pkt(-1, "FIN", framesSent + 1);
            udt_send(&pkt, targs->sockfd, &(targs->addr));

            
            end = time(NULL);
            printf("Network simulation finnished in: %ld ms\n", end - start);
            fflush(stdout);
            usleep(1000);
            runThreads = false;
        }
        rdt_rcv(&rcvpkt, targs->sockfd, &(targs->addr));

        pthread_mutex_lock(&mutex); // Lock the mutex

        printPacket(rcvpkt);
        if (rcvpkt.ACK == 1 && !checkCorrupt(rcvpkt)) // If the packet is an ACK
        {
            if (base == rcvpkt.seqNum) // If the ACK is for the packet at the base of the buffer
            {
                //InitPacket(&sndpkt[base]);     // Clear the packet from the buffer
                stop_timer(rcvpkt.seqNum);     // Stop the timer for the packet
                base = (base + 1) % WINSIZE; // Move the base forward
                successACK(rcvpkt.seqNum);
                slidingWindow();
                fflush(stdout);

                // Check for any out of order ACKs
                while (ACK_buffer[base])
                {
                    ACK_buffer[base] = 0;
                    stop_timer(base);
                    blueMSG("Removing out of order ACK for packet sequencenumber");
                    printf(" %d\n", base);
                    base = (base + 1) % WINSIZE;
                    slidingWindow();
                    fflush(stdout);
                }
            }

            // Buffer out of order ACKs
            else
            {
                successACK(rcvpkt.seqNum);
                //InitPacket(&sndpkt[rcvpkt.seqNum]);

                blueMSG("Buffering out of order ACK for packet sequencenumber: ");
                printf(" %d\n", rcvpkt.seqNum);

                ACK_buffer[rcvpkt.seqNum] = 1;
                stop_timer(rcvpkt.seqNum);
                slidingWindow();
                fflush(stdout);
            }
            ackCount++;
        }
        else // if Packet isn't an ACK its a NACK or corrupted
        {
            udt_send(&sndpkt[rcvpkt.seqNum], targs->sockfd, &(targs->addr));
            restart_timer(targs, rcvpkt.seqNum); // Restart the timer for the packet
            slidingWindow();
            fflush(stdout);
        }
        pthread_mutex_unlock(&mutex); // Unlock the mutex
    }
}

int main()
{
    socklen_t rcvAddrLen;
    base = 0;
    nextSeqNum = 0;

    runThreads = true;
    // printf("One small step for dev...\n");
    srand(time(NULL));

    for (int i = 0; i < NUMFRAMES; i++)
    {
        InitPacket(&sndpkt[i]);
    }

    // Create a UDP socket
    sendTargs.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendTargs.sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // printf("A plethora of bugs to fix!\n");

    // Set up server address
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

    pthread_mutex_init(&mutex, NULL);
    pthread_create(&sendThread, NULL, (void *)sendData, (void *)&sendTargs);
    pthread_create(&rcvThread, NULL, (void *)rcvData, (void *)&sendTargs);

    pthread_join(rcvThread, NULL);
    pthread_join(rcvThread, NULL);

    timeout(&sendTargs);

    ServerTeardown(sendTargs.sockfd, (struct sockaddr *)&sendTargs.addr, &rcvAddrLen);

    close(sendTargs.sockfd);
    pthread_mutex_destroy(&mutex);

    return 0;
}
