#include "Teardown.h"

void ClientTeardown(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    printf("Client Teardown Initiated\n");
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    while(1)
    {
        if(ReceiveFIN(fd, addr, addrLen)) break;
        printf("FIN not received\n");
    }
    printf("FIN received\n");

    SendFINACK(fd, addr, *addrLen);
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //Setting timeout for recvfrom()

    while (1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break; //If the condition is not met, it has timed out or something has gone horribly wrong
        SendFINACK(fd, addr, *addrLen);
        printf("ACK not received\n");
    }
    
    timeout.tv_sec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    printf(GRN "Teardown Complete\n" RESET);
}

void ServerTeardown(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    printf("Server Teardown Initiated\n");
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    time_t startTime;

    SendFIN(fd, addr, *addrLen);
    printf("FIN sent\n");
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //Setting timeout for recvfrom()

    while (1)
    {
        if(ReceiveFINACK(fd, addr, addrLen)) break;
        printf("Did not receive FINACK. Resending...\n");
        SendFIN(fd, addr, *addrLen);
    }

    SendACK(fd, addr, *addrLen);
    printf("ACK sent\n");

    StartTimer(&startTime);
    while (1)
    {
        if(ReceiveFINACK(fd, addr, addrLen)) SendACK(fd, addr, *addrLen);
        if(CheckTime(startTime, TIMEOUTLONG)) break;
    }

    timeout.tv_sec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    printf(GRN "Did not receive FINACK, assuming teardown is complete\n" RESET);
}

void SendFIN(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    SendFlagPacket(fd, destAddr, addrLen, "0010");
}

void SendFINACK(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    SendFlagPacket(fd, destAddr, addrLen, "1010");
}

int ReceiveFIN(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    return ReceiveFlagPacket(fd, src_addr, addrLen, "0010");
}

int ReceiveFINACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    return ReceiveFlagPacket(fd, src_addr, addrLen, "1010");
}