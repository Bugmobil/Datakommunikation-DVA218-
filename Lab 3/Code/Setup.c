#include "Setup.h"

int timeout;

void ClientSetup(int fd, const struct sockaddr* addr, socklen_t addrLen)
{
    timeout = TIMEOUT;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    SendSYN(fd, addr, addrLen);
    //Add loop
    ReceiveSYNACK(fd, addr, addrLen);
    SendACK(fd, addr, addrLen);
    timeout = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

void SendSYN(int fd, const struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet synPkt;
    char serPkt[MAXMSG];
    InitPacket(synPkt);
    synPkt.SYN = 1;
    Serialize(serPkt, synPkt);
    sendto(fd, serPkt, MAXMSG, 0, destAddr, addrLen);
}

void SendACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet ackPkt;
    char serPkt[MAXMSG];
    InitPacket(ackPkt);
    ackPkt.ACK = 1;
    Serialize(serPkt, ackPkt);
    sendto(fd, serPkt, MAXMSG, 0, destAddr, addrLen);
}

void SendSYNACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet synAckPkt;
    char serPkt[MAXMSG];
    InitPacket(synAckPkt);
    synAckPkt.SYN = 1;
    synAckPkt.ACK = 1;
    Serialize(serPkt, synAckPkt);
    sendto(fd, serPkt, MAXMSG, 0, destAddr, addrLen);
}

int ReceiveSYN(int fd, struct sockaddr* src_addr, socklen_t* addrlen)
{
    char buffer[2];
    //if(recvfrom(fd, ));
}

int ReceiveACK(int fd, struct sockaddr* src_addr, socklen_t* addrlen);
int ReceiveSYNACK(int fd, struct sockaddr* src_addr, socklen_t* addrlen);