#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdint.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[] = "Hello from client";

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Configure settings in address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send data to server
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Close the socket
    close(sockfd);

    return 0;
    
}