#include "Utils.h"
#include "Setup.h"

int main(int argc, char *argv[])
{
    int sockfd;
    char hostName[hostNameLength];
    struct sockaddr_in server_addr;
    struct hostent* hostInfo;
    Packet packet;

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

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    hostInfo = gethostbyname(hostName);

    // Configure settings in address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *(struct in_addr *)hostInfo->h_addr_list[0];

    // Close the socket
    close(sockfd);

    return 0;

}