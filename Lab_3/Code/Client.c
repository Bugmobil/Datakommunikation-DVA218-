#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Utils.h"

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAXMSG];

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    while (1)
    {
        printf("Enter a message to send to the server: ");
        fgets(buffer, MAXMSG, stdin);

        int send_len = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        if (send_len < 0)
        {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }
    }

    close(sockfd);
    return 0;
}
