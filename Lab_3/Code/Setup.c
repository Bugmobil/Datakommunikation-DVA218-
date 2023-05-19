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
    printf("Client Setup Initiated\n");

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    time_t startTime;
    errorLocation(__FUNCTION__, __FILE__, __LINE__);
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("Error");
    }

    sleep(1);

    while(1)
    {
        SendSYN(fd, addr, *addrLen);
        printf("SYN sent\n");
        if(ReceiveSYNACK(fd, addr, addrLen)) break;
    }

    printf("SYNACK Received\n");

    SendACK(fd, addr, *addrLen);

    StartTimer(&startTime);
    while(1)
    {
        if(ReceiveSYNACK(fd, addr, addrLen))
        {
            SendACK(fd, addr, *addrLen);
            StartTimer(&startTime);
        }
        else if(CheckTime(startTime, TIMEOUTLONG)) break;
    }

    timeout.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    printf(GRN "Did not receive SYNACK, assuming connection is complete\n" RESET);
}

void ServerSetup(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    printf("Server Setup Initiated\n");

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    while(1)
    {
        if(ReceiveSYN(fd, addr, addrLen))
        {
            printf("SYN Received\n");
            SendSYNACK(fd, addr, *addrLen);
            break;
        }
        else
            printf("Received bogus\n");
    }

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while(1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break;
        SendSYNACK(fd, addr, *addrLen);
    }

    timeout.tv_sec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    printf(GRN "Setup Complete\n" RESET);
}

void SendSYN(int fd, struct sockaddr* destAddr, socklen_t addrLen)
{
    SendFlagPacket(fd, destAddr, addrLen, "0100");
}

void SendACK(int fd, struct sockaddr *destAddr, socklen_t addrLen)
{
    SendFlagPacket(fd, destAddr, addrLen, "1000");
}

void SendSYNACK(int fd, struct sockaddr *destAddr, socklen_t addrLen)
{
    SendFlagPacket(fd, destAddr, addrLen, "1100");
}

int ReceiveSYN(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    return ReceiveFlagPacket(fd, src_addr, addrLen, "0100");
}

int ReceiveACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    return ReceiveFlagPacket(fd, src_addr, addrLen, "1000");
}

int ReceiveSYNACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen)
{
    return ReceiveFlagPacket(fd, src_addr, addrLen, "1100");
}
