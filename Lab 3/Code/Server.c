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

int main () {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[1024];

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Configure settings in address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the server address
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive data from client
    addr_len = sizeof(client_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);

    // Process the received data 
    printf("Data received from client: %s", buffer);

    return 0;
}