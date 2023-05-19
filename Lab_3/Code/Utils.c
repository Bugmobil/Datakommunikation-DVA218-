/*
 ============================================================================
 Name        : Utils.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This file contains the implementation of the utility functions
               declared in utils.h.
 ============================================================================
 */

#include "Utils.h"

char *testData = "This is the frames that we want to send to the server in correct order and without errors in the data or the ACKs";

void InitPacket(Packet *packet)
{
    packet->ACK = 0;
    packet->SYN = 0;
    packet->FIN = 0;
    packet->NACK = 0;
    packet->data[0] = '\0';
    packet->dataSize = 0;
    packet->seqNum = 0;
    packet->timestamp = 0;
    packet->checksum = 0;
}

void Serialize(char *serializedPacket, Packet packet)
{
    uint16_t flags = 0;
    uint32_t dataSize = htonl(packet.dataSize);
    uint32_t seqNum = htonl(packet.seqNum);
    uint32_t timestamp = htonl(packet.timestamp);
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
    memcpy(serializedPacket + messageLength + sizeof(uint16_t), &dataSize, sizeof(uint32_t));
    memcpy(serializedPacket + messageLength + sizeof(uint16_t) + sizeof(uint32_t), &seqNum, sizeof(uint32_t));
    memcpy(serializedPacket + messageLength + sizeof(uint16_t) + 2 * sizeof(uint32_t), &timestamp, sizeof(uint32_t));
    memcpy(serializedPacket + messageLength + sizeof(uint16_t) + 3 * sizeof(uint32_t), &checksum, sizeof(uint32_t));
}

void Deserialize(char *serializedPacket, Packet *packet)
{
    uint16_t flags;
    uint32_t dataSize;
    uint32_t seqNum;
    uint32_t timestamp;
    uint32_t checksum;

    memcpy(&flags, serializedPacket, sizeof(uint16_t));
    memcpy(&packet->data, serializedPacket + sizeof(uint16_t), packet->dataSize);
    memcpy(&dataSize, serializedPacket + messageLength + sizeof(uint16_t), sizeof(uint32_t));
    memcpy(&seqNum, serializedPacket + messageLength + sizeof(uint16_t) + sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&timestamp, serializedPacket + messageLength + sizeof(uint16_t) + 2 * sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&checksum, serializedPacket + messageLength + sizeof(uint16_t) + 3 * sizeof(uint32_t), sizeof(uint32_t));

    packet->ACK = 1 & (flags >> 0);
    packet->SYN = 1 & (flags >> 1);
    packet->FIN = 1 & (flags >> 2);
    packet->NACK = 1 & (flags >> 3);

    packet->dataSize = ntohl(dataSize);
    packet->seqNum = ntohl(seqNum);
    packet->timestamp = ntohl(timestamp);
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
}

//TODO FIX THIS. Nr1 Priority
int ReceiveFlagPacket(int fd, struct sockaddr *src_addr, socklen_t *addrLen, const char* flags)
{
    Packet packet;
    char buffer[PACKET_SIZE];
    InitPacket(&packet);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &packet);

        printf("ACK: %d == %d \t SYN: %d == %d \t FIN: %d == %d \t NACK: %d == %d\n", 
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

void StartTimer(time_t* startTime)
{
    *startTime = time(NULL);
}

int CheckTime(time_t startTime, int timeout)
{
    time_t currentTime;
    time(&currentTime);
    return (currentTime - startTime >= timeout) ? 1 : 0;
}

int GiveRandomNumber(int from, int to)
{
	return rand() % (to - from + 1) + from;
}

void CorruptPacket(char* packet)
{
    int errorRate;
    srand(time(NULL));
    errorRate = GiveRandomNumber(33, 67);
    CorruptPacketPercentage(packet, errorRate);
}

void CorruptPacketPercentage(char* packet, int errorRate)
{
    size_t i;
    srand(time(NULL));

    for (i = 0; i < PACKET_SIZE; i++)
    {
        if(packet[i] != '\0' && GiveRandomNumber(1, 100) <= errorRate)
        {
            packet[i] ^= (1 << (rand() % 8));
        }
    }
}

void printPacket (Packet pkt)
{
    printf("┌ ・・・・・・・・・・・・・・ ┐");
    printf("┊DATA: %s\n", pkt.data);
    printf("┊SEQ NUM: %d\n", pkt.seqNum);
    printf("┊ACK/NACK: %d/%d\n", pkt.ACK, pkt.NACK);
    printf("┊CHECKSUM: %d\n", pkt.checksum);
    printf("└ ・・・・・・・・・・・・・・ ┘");
}

/* Error Handling */
void errorLocation(char *function, char *file, int line)
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

void successMSG(char *msg)
{
    printf("Function %s executed", msg);
    printf(GRN " successfully\n" RESET);
}

void corruptedMSG(int seqNum)
{
    printf(RED "Corrupted packet! " RESET);
    printf("Sequence number: ");
    printf(MAG " %d\n" RESET, seqNum);
}
/* End of Error Handling */