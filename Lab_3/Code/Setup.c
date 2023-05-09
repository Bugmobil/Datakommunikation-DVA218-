#include "Setup.h"

void ClientSetup(int fd, const struct sockaddr* addr, socklen_t addrLen)
{
    int timeout = TIMEOUT;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    SendSYN(fd, addr, addrLen);
    ReceiveSYNACK(fd, addr, addrLen);
    SendACK(fd, addr, addrLen);
    timeout = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

void SendSYN(int fd, const struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet synPkt;
    char serPkt[BUFFER_SIZE];
    InitPacket(&synPkt);
    synPkt.SYN = 1;
    Serialize(serPkt, synPkt);
    sendto(fd, serPkt, BUFFER_SIZE, 0, destAddr, addrLen);
}

void SendACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet ackPkt;
    char serPkt[BUFFER_SIZE];
    InitPacket(&ackPkt);
    ackPkt.ACK = 1;
    Serialize(serPkt, ackPkt);
    sendto(fd, serPkt, BUFFER_SIZE, 0, destAddr, addrLen);
}

void SendSYNACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet synAckPkt;
    char serPkt[BUFFER_SIZE];
    InitPacket(&synAckPkt);
    synAckPkt.SYN = 1;
    synAckPkt.ACK = 1;
    Serialize(serPkt, synAckPkt);
    sendto(fd, serPkt, BUFFER_SIZE, 0, destAddr, addrLen);
}