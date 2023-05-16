/*
 ============================================================================
 Name        : Setup.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This file contains the implementation of the functions declared
               in Setup.h. It should include the logic for connection setup.
 ============================================================================
 */
#include "Setup.h"

int timeout;

int timeout;

void ClientSetup(int fd, const struct sockaddr *addr, socklen_t addrLen)
{
    time_t startTime;
    time_t currentTime;
    timeout = TIMEOUT * 1000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while(1)
    {
        SendSYN(fd, addr, addrLen);
        if(ReceiveSYNACK(fd, addr, addrLen)) break;
    }

    SendACK(fd, addr, addrLen);

    time(&startTime);
    while(1)
    {
        if(ReceiveSYNACK(fd, addr, addrLen)) SendACK(fd, addr, addrLen);
        time(&currentTime);
        if(currentTime - startTime >= TIMEOUTLONG) break;
    }

    timeout = 0;
}

void SendSYN(int fd, const struct sockaddr *destAddr, socklen_t addrLen)
{
    Packet synPkt;
    char serPkt[BUFFER_SIZE];
    InitPacket(&synPkt);
    synPkt.SYN = 1;
    Serialize(serPkt, synPkt);
    sendto(fd, serPkt, BUFFER_SIZE, 0, destAddr, addrLen);
}

void SendACK(int fd, const struct sockaddr *destAddr, socklen_t addrLen)
{
    Packet ackPkt;
    char serPkt[BUFFER_SIZE];
    InitPacket(&ackPkt);
    ackPkt.ACK = 1;
    Serialize(serPkt, ackPkt);
    sendto(fd, serPkt, BUFFER_SIZE, 0, destAddr, addrLen);
}

void SendSYNACK(int fd, const struct sockaddr *destAddr, socklen_t addrLen)
{
    Packet synAckPkt;
    char serPkt[BUFFER_SIZE];
    InitPacket(&synAckPkt);
    synAckPkt.SYN = 1;
    synAckPkt.ACK = 1;
    Serialize(serPkt, synAckPkt);
    sendto(fd, serPkt, BUFFER_SIZE, 0, destAddr, addrLen);
}

int ReceiveSYN(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet synPkt;
    char buffer[BUFFER_SIZE];
    InitPacket(&synPkt);
    if(recvfrom(fd, buffer, BUFFER_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &synPkt);
        if(synPkt.SYN) return 1;
    }

    return 0;
}

int ReceiveACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet ackPkt;
    char buffer[BUFFER_SIZE];
    InitPacket(&ackPkt);
    if(recvfrom(fd, buffer, BUFFER_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &ackPkt);
        if(ackPkt.ACK) return 1;
    }

    return 0;
}

int ReceiveSYNACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet synAckPkt;
    char buffer[BUFFER_SIZE];
    InitPacket(&synAckPkt);
    if(recvfrom(fd, buffer, BUFFER_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &synAckPkt);
        if(synAckPkt.SYN && synAckPkt.ACK) return 1;
    }

    return 0;
}
