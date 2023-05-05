/*
This file contains the implementation of the functions declared in client.h. 
It should handle the client-side logic for connecting to a server, sending data, and receiving acknowledgments.
*/

#include "Utils.h"

int main(int argc, char *argv[])
{
    int sock;
    char hostName[hostNameLength];
    Packet packetToSend;

    printf("Fueled up, ready to go!\n");

    if(argv[1] == NULL)
    {
        perror("Usage: client [host name]\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        strncpy(hostName, argv[1], hostNameLength);
        hostName[hostNameLength - 1] = '\0';
    }
}