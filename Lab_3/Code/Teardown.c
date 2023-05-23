#include "Teardown.h"

void ClientTeardown(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    printf("Client Teardown Initiated\n");
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TIMEOUTUSEC;

    //Waits for FIN in which it will later send a FIN and ACK. If it receives something else it will ignore it.
    /*while(1)
    {
        if(ReceiveFIN(fd, addr, addrLen)) break;
        printf("FIN not received\n");
    }
    printf("FIN received\n");*/
    SendFINACK(fd, addr, *addrLen);
    
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) //Setting timeout for recvfrom()
        perror("Error Setting Timeout");

    //Waits for ACK. If it times out it resends FIN and ACK. Will loop until ACK is received
    while (1)
    {
        if(ReceiveACK(fd, addr, addrLen)) break;
        SendFINACK(fd, addr, *addrLen);
        printf("ACK not received\n");
    }
    
    //Reset the timeout to infinite
    timeout.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        perror("Error Setting Timeout");
    printf(GRN "Teardown Complete\n" RESET);
}

void ServerTeardown(int fd, struct sockaddr* addr, socklen_t* addrLen)
{
    printf("Server Teardown Initiated\n");
    struct timeval startTime;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TIMEOUTUSEC;

    /*SendFIN(fd, addr, *addrLen);
    printf("FIN sent\n");*/
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) //Setting timeout for recvfrom()
        perror("Error Setting Timeout");

    //Waiting to receive a FIN and ACK. If it times out it will resend the FIN. Will loop until FIN and ACK has come through.
    while (1)
    {
        if(ReceiveFINACK(fd, addr, addrLen)) break;
        printf("Did not receive FINACK. Resending...\n");
        SendFIN(fd, addr, *addrLen);
    }

    SendACK(fd, addr, *addrLen);
    printf("ACK sent\n");
    StartTimer(&startTime);

    //Final loop where the server waits in case the ACK paket was lost or corrupted
    while (1)
    {
        if(ReceiveFINACK(fd, addr, addrLen))
        {
            SendACK(fd, addr, *addrLen);
            StartTimer(&startTime);
        } 
        else if(CheckTime(startTime, TIMEOUTLONG)) break;
    }

    //Reset the timeout to infinite
    timeout.tv_sec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        perror("Error Setting Timeout");
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