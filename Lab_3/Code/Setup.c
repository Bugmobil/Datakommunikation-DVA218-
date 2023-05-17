/*
 ============================================================================
 Name        : Setup.c
 Authors     : Majid Azizi (mai20018) & Andreas Pearson (apn20017)
 Description : This file contains the implementation of the functions declared
               in Setup.h. It should include the logic for connection setup.
 ============================================================================
 */
#include "Setup.h"

void ClientSetup(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    int timeout = TIMEOUT * 1000;
    time_t startTime;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while(1)
    {
        SendSYN(fd, addr, *addrLen);
        if(ReceiveSYNACK(fd, addr, addrLen)) break;
    }

    SendACK(fd, addr, *addrLen);

    StartTimeout(&startTime);
    while(1)
    {
        if(ReceiveSYNACK(fd, addr, addrLen)) SendACK(fd, addr, *addrLen);
        if(CheckTimeout(startTime, TIMEOUTLONG)) break;
    }

    timeout = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

void ServerSetup(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    int timeout = TIMEOUT * 1000;

    if(ReceiveSYN(fd, addr, addrLen))
    {
        SendSYNACK(fd, addr, *addrLen);
    }

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while(1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break;
        SendSYNACK(fd, addr, *addrLen);
    }

    timeout = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

void SendSYN(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet synPkt;
    char serPkt[PACKET_SIZE];
    InitPacket(&synPkt);
    synPkt.SYN = 1;
    Serialize(serPkt, synPkt);
    sendto(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
}

void SendACK(int fd, struct sockaddr *destAddr, socklen_t addrLen)
{
    Packet ackPkt;
    char serPkt[PACKET_SIZE];
    InitPacket(&ackPkt);
    ackPkt.ACK = 1;
    Serialize(serPkt, ackPkt);
    sendto(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
}

void SendSYNACK(int fd, struct sockaddr *destAddr, socklen_t addrLen)
{
    Packet synAckPkt;
    char serPkt[PACKET_SIZE];
    InitPacket(&synAckPkt);
    synAckPkt.SYN = 1;
    synAckPkt.ACK = 1;
    Serialize(serPkt, synAckPkt);
    sendto(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
}

int ReceiveSYN(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet synPkt;
    char buffer[PACKET_SIZE];
    InitPacket(&synPkt);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &synPkt);
        return (synPkt.SYN) ? 1 : 0;
    }

    return 0;
}

int ReceiveACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet ackPkt;
    char buffer[PACKET_SIZE];
    InitPacket(&ackPkt);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &ackPkt);
        return (ackPkt.ACK) ? 1 : 0;
    }

    return 0;
}

int ReceiveSYNACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet synAckPkt;
    char buffer[PACKET_SIZE];
    InitPacket(&synAckPkt);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &synAckPkt);
        return (synAckPkt.SYN && synAckPkt.ACK) ? 1 : 0;
    }

    return 0;
}
