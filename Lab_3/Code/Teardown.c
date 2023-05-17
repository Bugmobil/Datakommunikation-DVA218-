#include "Teardown.h"

void ClientTeardown(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    int timeout = TIMEOUT * 1000;

    if(ReceiveFIN(fd, addr, addrLen)) SendFINACK(fd, addr, *addrLen);

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //Setting timeout for recvfrom()

    while (1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break; //If the condition is not met, it has timed out or something has gone horribly wrong
        SendFINACK(fd, addr, *addrLen);
    }
    
    timeout = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

void ServerTeardown(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    int timeout = TIMEOUT * 1000;
    time_t startTime;

    SendFIN(fd, addr, *addrLen);

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //Setting timeout for recvfrom()

    while (1)
    {
        if(ReceiveFINACK(fd, addr, addrLen)) break;
        SendFIN(fd, addr, *addrLen);
    }

    SendACK(fd, addr, *addrLen);

    StartTimer(&startTime);
    while (1)
    {
        if(ReceiveFINACK(fd, addr, addrLen)) SendACK(fd, addr, *addrLen);
        if(CheckTime(startTime, TIMEOUTLONG)) break;
    }

    timeout = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
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