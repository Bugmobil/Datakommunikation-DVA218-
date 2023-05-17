#include "Teardown.h"

void ClientTeardown(int fd, struct sockaddr *addr, socklen_t addrLen);

void ServerTeardown(int fd, struct sockaddr* destAddr, socklen_t addrLen);

void SendFIN(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet finPkt;
    char serPkt[PACKET_SIZE];
    InitPacket(&finPkt);
    finPkt.FIN = 1;
    Serialize(serPkt, finPkt);
    sendto(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
}

void SendFINACK(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    Packet finAckPkt;
    char serPkt[PACKET_SIZE];
    InitPacket(&finAckPkt);
    finAckPkt.FIN = 1;
    finAckPkt.ACK = 1;
    Serialize(serPkt, finAckPkt);
    sendto(fd, serPkt, PACKET_SIZE, 0, destAddr, addrLen);
}

int ReceiveFIN(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet finPkt;
    char buffer[PACKET_SIZE];
    InitPacket(&finPkt);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &finPkt);
        if(finPkt.FIN) return 1;
    }

    return 0;
}

int ReceiveFINACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    Packet finAckPkt;
    char buffer[PACKET_SIZE];
    InitPacket(&finAckPkt);
    if(recvfrom(fd, buffer, PACKET_SIZE, 0, src_addr, addrLen) != -1)
    {
        Deserialize(buffer, &finAckPkt);
        if(finAckPkt.FIN && finAckPkt.ACK) return 1;
    }

    return 0;
}