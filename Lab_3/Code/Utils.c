/*
 ============================================================================
 Name        : Utils.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This file contains the implementation of the utility functions
               declared in utils.h.
 ============================================================================
 */

#include "Utils.h"

char *testData = "These are the frames that we want to send to the server in correct order and without errors in the data or the ACKs";
pthread_t sendThread;

int miliToMicro(int mili)
{
    return mili * 1000;
}
void InitPacket(Packet *packet)
{
    packet->ACK = 0;
    packet->SYN = 0;
    packet->FIN = 0;
    packet->NACK = 0;
    packet->data[0] = '\0';
    packet->dataSize = 0;
    packet->seqNum = -1;
    //packet->ID = 0;
    packet->checksum = 0;
}
void Serialize(char *serializedPacket, Packet packet)
{
    //printf("Entering Serialize\n");
    uint16_t flags = 0;
    uint32_t dataSize = htonl(packet.dataSize);
    uint32_t seqNum = htonl(packet.seqNum);
    //uint32_t timestamp = htonl(packet.ID);
    uint32_t checksum = htonl(packet.checksum);

    if (packet.ACK)
        flags |= (1 << 0);
    if (packet.SYN)
        flags |= (1 << 1);
    if (packet.FIN)
        flags |= (1 << 2);
    if (packet.NACK)
        flags |= (1 << 3);

    memcpy(serializedPacket, &flags, sizeof(uint16_t));
    memcpy(serializedPacket + sizeof(uint16_t), &packet.data, packet.dataSize);
    memcpy(serializedPacket + FRAMESIZE + sizeof(uint16_t), &dataSize, sizeof(uint32_t));
    memcpy(serializedPacket + FRAMESIZE + sizeof(uint16_t) + sizeof(uint32_t), &seqNum, sizeof(uint32_t));
    memcpy(serializedPacket + FRAMESIZE + sizeof(uint16_t) + 2 * sizeof(uint32_t), &checksum, sizeof(uint32_t));

    //printf("Serialize Complete\n");
}
void Deserialize(char *serializedPacket, Packet *packet)
{
    uint16_t flags;
    uint32_t dataSize;
    uint32_t seqNum;
    //uint32_t timestamp;
    uint32_t checksum;

    memcpy(&flags, serializedPacket, sizeof(uint16_t));
    memcpy(&dataSize, serializedPacket + FRAMESIZE + sizeof(uint16_t), sizeof(uint32_t));
    memcpy(&seqNum, serializedPacket + FRAMESIZE + sizeof(uint16_t) + sizeof(uint32_t), sizeof(uint32_t));
    //memcpy(&timestamp, serializedPacket + FRAMESIZE + sizeof(uint16_t) + 2 * sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&checksum, serializedPacket + FRAMESIZE + sizeof(uint16_t) + 2 * sizeof(uint32_t), sizeof(uint32_t));

    packet->dataSize = ntohl(dataSize);
    memcpy(&packet->data, serializedPacket + sizeof(uint16_t), packet->dataSize);

    packet->ACK = 1 & (flags >> 0);
    packet->SYN = 1 & (flags >> 1);
    packet->FIN = 1 & (flags >> 2);
    packet->NACK = 1 & (flags >> 3);

    packet->seqNum = ntohl(seqNum);
    //packet->ID = ntohl(timestamp);
    packet->checksum = ntohl(checksum);
}

void SendFlagPacket(int fd, struct sockaddr *destAddr, socklen_t addrLen, const char* flags)
{
    Packet packet;
    char serPkt[PACKET_SIZE];
    InitPacket(&packet);
    packet.ACK = flags[0] & 1;
    packet.SYN = flags[1] & 1;
    packet.FIN = flags[2] & 1;
    packet.NACK = flags[3] & 1;
    Serialize(serPkt, packet);
    sendto(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
    //SendFaulty(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
}
int ReceiveFlagPacket(int fd, struct sockaddr *src_addr, socklen_t *addrLen, const char* flags)
{
    Packet packet;
    char buffer[PACKET_SIZE];
    InitPacket(&packet);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &packet);

        printf("Received: ACK: %d == %d \t SYN: %d == %d \t FIN: %d == %d \t NACK: %d == %d\n", 
        packet.ACK, flags[0] & 1, packet.SYN, flags[1] & 1, packet.FIN, flags[2] & 1, packet.NACK, flags[3] & 1);

        return 
        ((packet.ACK == (flags[0] & 1)) 
        & (packet.SYN == (flags[1] & 1)) 
        & (packet.FIN == (flags[2] & 1)) 
        & (packet.NACK == (flags[3] & 1))) 
        ? 1 : 0;
    }

    return 0;
}

void StartTimer(struct timeval* startTime)
{
    gettimeofday(startTime, NULL);
}
int CheckTime(struct timeval startTime, int timeout)
{
    struct timeval currentTime;
    long elapsedTime = 0;
    gettimeofday(&currentTime, NULL);

    elapsedTime += (currentTime.tv_sec - startTime.tv_sec) * 1000;  // Convert seconds to milliseconds
    elapsedTime += (currentTime.tv_usec - startTime.tv_usec) / 1000;     // Convert microseconds to milliseconds

    return (elapsedTime >= timeout) ? 1 : 0;
}

int GiveRandomNumber(const int from, const int to)
{
	return rand() % (to - from + 1) + from;
}
void CorruptPacket(char* packet)
{
    int errorRate;
    errorRate = GiveRandomNumber(33, 67);
    CorruptPacketPercentage(packet, errorRate);
}
void CorruptPacketPercentage(char* packet, int errorRate)
{
    size_t i;
    for (i = 0; i < PACKET_SIZE; i++)
    {
        if(packet[i] != '\0' && GiveRandomNumber(1, 100) <= errorRate)
        {
            packet[i] ^= (1 << (GiveRandomNumber(1, 8)));
        }
    }
}

void ThreadSendDelay(ThreadSend* packet)
{
    usleep(PROPDELAY * 1000);
    printf("Sending: %ld == %d", sizeof(packet->buffer), packet->size);
    sendto(packet->fd, packet->buffer, packet->size, 0, packet->destAddr, packet->addrLen);
    printf("Sent\n");
    free(packet);
    printf("Freed\n");
}
void SendFaulty(int fd, char* buffer, int size, int flags, struct sockaddr *destAddr, socklen_t addrLen)
{
    //ThreadSend* packet = malloc(sizeof(ThreadSend));
    if(GiveRandomNumber(1, 100) > ERRORRATE)
    {
        if(GiveRandomNumber(1, 100) <= ERRORRATE)
        {
            printf("Corrupting Packet...\n");
            CorruptPacket(buffer);
        }

        sendto(fd, buffer, size, 0, destAddr, addrLen);
        /*
        packet->fd = fd;
        printf("Pre memcpy()\n");
        memcpy(packet->buffer, buffer, PACKET_SIZE);
        packet->size = size;
        packet->flags = flags;
        packet->destAddr = destAddr;
        packet->addrLen = addrLen;
        printf("Post memcpy()\n");
        pthread_create(&sendThread, NULL, (void *)ThreadSendDelay, (ThreadSend*) packet);
        */
    }
}

void printPacket(Packet pkt)
{
    printf(BLU "\n┌ ・・・・・・・・・・・・・・ ┐\n"RESET);
    printf("┊DATA: %s\n", pkt.data);
    printf("┊SEQ NUM: %d\n", pkt.seqNum);
    printf("┊ACK/NACK: %d/%d\n", pkt.ACK, pkt.NACK);
    printf("┊CHECKSUM: %d\n", pkt.checksum);
    printf(BLU "└ ・・・・・・・・・・・・・・ ┘\n"RESET);
}

void execMSG()
{
    printf(MAG "\n┌ ・・・・・・・・・・・・・・ ┐\n"RESET);
    printf("┊ Simulation started!\n");
    printf("┊ Number of frames: %d\n", NUMFRAMES);
    printf("┊ Frame size: %d byte\n", FRAMESIZE);
    printf("┊ Window size: %d\n", WINSIZE);
    printf("┊ Error rate: %d%%\n", ERRORRATE);
    printf(MAG "└ ・・・・・・・・・・・・・・ ┘\n"RESET);

}


void printTime()
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf(MAG "\n┌ ・・・・・・・・・・・・・・ ┐\n"RESET);
    printf("┊ %s", asctime (timeinfo) );
    printf(MAG "└ ・・・・・・・・・・・・・・ ┘\n"RESET);
}

/* Error Handling */
void errorLocation(const char *function, const char *file, int line)
{
    printf(MAG "Function: %s\nFile: %s, Line: %d\n" RESET, function, file, line);
    printf(RESET"");
}
void errorMSG(char *msg)
{
    printf(RED "Error in function: %s\n" RESET, msg);
    perror(NULL); // Print the error message corresponding to errno
    exit(EXIT_FAILURE);
}
void warningMSG(char *func, char *problem)
{
    printf(YEL "Warning in function: %s" RESET, func);
    printf("Problem: %s\n", problem);
}
void successPKT(int seqNum)
{
    printf(GRN "Packet successfully sent! " RESET);
    printf("Sequence number: ");
    printf(MAG " %d\n" RESET, seqNum);
}

void successMSG(char *msg)
{
    printf("%s: ", msg);
    printf(GRN "PASSED\n" RESET);
}

void failMSG(char *msg)
{
    printf("%s: ", msg);
    printf(RED "FAILED\n" RESET);
}
void successACK(int seqNum)
{
    printf(GRN "Received ACK for packet with " RESET);
    printf("Sequence number: ");
    printf(MAG " %d\n" RESET, seqNum);
}
void blueMSG(char *msg)
{
    printf(BLU "%s" RESET, msg);
}
/* End of Error Handling */