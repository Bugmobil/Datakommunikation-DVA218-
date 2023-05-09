
#include "Utils.h"
#include "udp_transport.h"

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char message[messageLength];

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
        fgets(message, messageLength, stdin);
        message[strcspn(message, "\n")] = '\0';

        if (nextSeqNum < base + N)
        {
            sndpkt[nextSeqNum] = make_pkt(nextSeqNum, message, checksum(message, strlen(message)));
            udt_send(&sndpkt[nextSeqNum], sockfd, SERVER_IP);
            start_timer(nextSeqNum);
            nextSeqNum = (nextSeqNum + 1) % MAXSEQ;
        }
        else
        {
            refuse_data(message);
        }
    }

    close(sockfd);
    return 0;
}
