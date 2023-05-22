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
    struct timeval startTime;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TIMEOUTUSEC;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) //Setting timeout for recvfrom()
        perror("Error Setting Timeout");
    
    //Sends SYN and is waiting to receive a SYN and ACK. If it times out it will resend the SYN. Will loop until SYN and ACK has come through.
    printf("Sending SYN\n");
    while(1)
    {
        SendSYN(fd, addr, *addrLen);
        if(ReceiveSYNACK(fd, addr, addrLen)) break;
    }

    printf("SYNACK Received. Sending ACK...\n");
    SendACK(fd, addr, *addrLen);
    StartTimer(&startTime);

    //Final loop where the client waits in case the ACK paket was lost or corrupted
    while(1)
    {
        //Resends ACK if it receives SYN and ACK and resets timer
        if(ReceiveSYNACK(fd, addr, addrLen))
        {
            printf("SYNACK Received. Resending ACK...\n");
            SendACK(fd, addr, *addrLen);
            StartTimer(&startTime);
        }
        else if(CheckTime(startTime, TIMEOUTLONG)) break;
    }

    //Reset the timeout to infinite
    timeout.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        perror("Error Setting Timeout");

    printf(GRN "Did not receive SYNACK, assuming connection is complete\n" RESET);
}

void ServerSetup(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    printf("Server Setup Initiated\n");

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TIMEOUTUSEC;

    //Waits for SYN in which it will send a SYN and ACK. If it receives something else it will ignore it.
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

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) //Setting timeout for recvfrom()
        perror("Error Setting Timeout");

    //Waits for ACK. If it times out it resends SYN and ACK. Will loop until ACK is received
    while(1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break;
        SendSYNACK(fd, addr, *addrLen);
    }

    //Reset the timeout to infinite
    timeout.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        perror("Error Setting Timeout");
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