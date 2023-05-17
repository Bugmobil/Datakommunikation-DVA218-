#include "Teardown.h"

int timeout;

void ClientTeardown(int fd, struct sockaddr *addr, socklen_t addrLen)
{
    if(ReceiveFIN(fd, addr, addrLen)) SendFINACK(fd, addr, addrLen);

    timeout = TIMEOUT * 1000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //Setting timeout for recvfrom()

    while (1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break; //If the condition is not met, it has timed out or something has gone horribly wrong
        SendFINACK(fd, addr, addrLen);
    }
}

void ServerTeardown(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    time_t startTime;
    SendFIN(fd, destAddr, addrLen);

    while (1)
    {
        if(ReceiveFINACK(fd, destAddr, addrLen)) break;
        SendFIN(fd, destAddr, addrLen);
    }

    SendACK(fd, destAddr, addrLen);

    StartTimeout(startTime);
    while (1)
    {
        if(ReceiveFINACK(fd, destAddr, addrLen)) SendACK(fd, destAddr, addrLen);
        if(CheckTimeout(startTime, TIMEOUTLONG)) break;
    }
}

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
        return (finPkt.ACK) ? 1 : 0;
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
        return (finAckPkt.FIN && finAckPkt.ACK) ? 1 : 0;
    }

    return 0;
}