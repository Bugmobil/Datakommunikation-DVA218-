/* File: server.c
 * Trying out socket communication between processes using the Internet protocol family.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 5555
#define MAXMSG 512

/* makeSocket
 * Creates and names a socket in the Internet
 * name-space. The socket created exists
 * on the machine from which the function is 
 * called. Instead of finding and using the
 * machine's Internet address, the function
 * specifies INADDR_ANY as the host address;
 * the system replaces that with the machine's
 * actual address.
 */
int makeSocket(unsigned short int port)
{
	int sock;
	struct sockaddr_in name;
int makeSocket(unsigned short int port)
{
	int sock;
	struct sockaddr_in name;

	/* Create a socket. */
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("Could not create a socket\n");
		exit(EXIT_FAILURE);
	}
	/* Give the socket a name. */
	/* Socket address format set to AF_INET for Internet use. */
	name.sin_family = AF_INET;
	/* Set port number. The function htons converts from host byte order to network byte order.*/
	name.sin_port = htons(port);
	/* Set the Internet address of the host the function is called from. */
	/* The function htonl converts INADDR_ANY from host byte order to network byte order. */
	/* (htonl does the same thing as htons but the former converts a long integer whereas
	* htons converts a short.)
	*/
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Assign an address to the socket by calling bind. */
	if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
	{
		perror("Could not bind a name to the socket\n");
		exit(EXIT_FAILURE);
	}
	return(sock);
	/* Create a socket. */
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("Could not create a socket\n");
		exit(EXIT_FAILURE);
	}
	/* Give the socket a name. */
	/* Socket address format set to AF_INET for Internet use. */
	name.sin_family = AF_INET;
	/* Set port number. The function htons converts from host byte order to network byte order.*/
	name.sin_port = htons(port);
	/* Set the Internet address of the host the function is called from. */
	/* The function htonl converts INADDR_ANY from host byte order to network byte order. */
	/* (htonl does the same thing as htons but the former converts a long integer whereas
	* htons converts a short.)
	*/
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Assign an address to the socket by calling bind. */
	if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
	{
		perror("Could not bind a name to the socket\n");
		exit(EXIT_FAILURE);
	}
	return(sock);
}


/* readMessageFromClient
 * Reads and prints data read from the file (socket
 * denoted by the file descriptor 'fileDescriptor'.
 */
int readMessageFromClient(int fileDescriptor) {
	char buffer[MAXMSG];
	int nOfBytes;
	nOfBytes = read(fileDescriptor, buffer, MAXMSG);
	if(nOfBytes < 0)
	{
		perror("Could not read data from client\n");
		exit(EXIT_FAILURE);
	}
	else
	if(nOfBytes == 0)
		/* End of file */
		return(-1);
	else
	{
		/* Data read */
		printf(">Incoming message: %s\n",  buffer);
	}
	return(0);
	char buffer[MAXMSG];
	int nOfBytes;
	nOfBytes = read(fileDescriptor, buffer, MAXMSG);
	if(nOfBytes < 0)
	{
		perror("Could not read data from client\n");
		exit(EXIT_FAILURE);
	}
	else
	if(nOfBytes == 0)
		/* End of file */
		return(-1);
	else
	{
		/* Data read */
		printf(">Incoming message: %s\n",  buffer);
	}
	return(0);
}

//Writes msg to client depending on msgType
int sendMSG(int fileDescriptor, int msgType) {
	int nOfBytes;
	//Acknowledge message
	if(msgType == 1)
	{
		char ACK[MAXMSG];
		strcpy(ACK, "I hear you loud and clear.");
		ACK[sizeof(ACK) - 1] = '\0';
		nOfBytes = write(fileDescriptor, ACK, strlen(ACK) + 1);
		if(nOfBytes < 0)
		{
			perror("Could not write data to client\n");
			exit(EXIT_FAILURE);
		}
	}
	//Blocked message
	else if(msgType == 2)
	{
		char Blocked[MAXMSG];
		strcpy(Blocked, "Access denied.");
		Blocked[sizeof(Blocked) - 1] = '\0';
		nOfBytes = write(fileDescriptor, Blocked, strlen(Blocked) + 1);
		if(nOfBytes < 0)
		{
			perror("Could not write data to client\n");
			exit(EXIT_FAILURE);
		}
	}
	return(0);
}

//Sends a message to all clients except the one just connected
void AnnounceNewClient(fd_set* activeFdSet, int fd, int clientFD)
{
	int i, nOfBytes;
	char msg[MAXMSG];
	strcpy(msg, "A new power is rising!");
	msg[sizeof(msg) - 1] = '\0';
	
	for(i = 0; i < FD_SETSIZE; ++i)	// Iterate through all file descriptors in the set
		if(FD_ISSET(i, activeFdSet)) // If the file descriptor is in the set 
		{
			if(i != fd && i != clientFD) // If the file descriptor is not the server or the client
			{
				nOfBytes = write(i, msg, MAXMSG); // Send the message to the client
				if(nOfBytes < 0)
				{
					perror("Could not write data to client\n");
					exit(EXIT_FAILURE);
				}
			}
		}
}

int main(int argc, char *argv[])
{
	int sock;
	int clientSocket;
	int i;
	fd_set activeFdSet, readFdSet; /* Used by select */
	struct sockaddr_in clientName;
	socklen_t size;
	const char *blocked_ip = "127.0.0.7";


	/* Create a socket and set it up to accept connections */
	sock = makeSocket(PORT);
	/* Listen for connection requests from clients */
	if(listen(sock, 1) < 0)
	{
		perror("Could not listen for connections\n");
		exit(EXIT_FAILURE);
	}
	/* Initialize the set of active sockets */
	FD_ZERO(&activeFdSet);
	FD_SET(sock, &activeFdSet);

	printf("\n[waiting for connections...]\n");
	while(1)
	{
		/* Block until input arrives on one or more active sockets
		FD_SETSIZE is a constant with value = 1024 */
		readFdSet = activeFdSet;
		if(select(FD_SETSIZE, &readFdSet, NULL, NULL, NULL) < 0)
		{
			perror("Select failed\n");
			exit(EXIT_FAILURE);
		}
		/* Service all the sockets with input pending */
		for(i = 0; i < FD_SETSIZE; ++i)
			if(FD_ISSET(i, &readFdSet))
			{
				if(i == sock)
				{
					/* Connection request on original socket */
					size = sizeof(struct sockaddr_in);
					/* Accept the connection request from a client. */
					clientSocket = accept(sock, (struct sockaddr *)&clientName, (socklen_t *)&size);
					if(clientSocket < 0)
					{
						perror("Could not accept connection\n");
						exit(EXIT_FAILURE);
					}

					printf("Server: Connect from client %s, port %d\n",
					inet_ntoa(clientName.sin_addr),
					ntohs(clientName.sin_port));
					FD_SET(clientSocket, &activeFdSet);
					//sendMSG(clientSocket, 3);
					AnnounceNewClient(&activeFdSet, sock, clientSocket);

					//Check if client ip is blocked
					char client_ip[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &(clientName.sin_addr), client_ip, INET_ADDRSTRLEN);

					if(strcmp(client_ip, blocked_ip) == 0)
					{
						printf("Server: Client-ip %s is blocked.\n", inet_ntoa(clientName.sin_addr));
						sendMSG(clientSocket, 2);
						close(clientSocket);
						FD_CLR(clientSocket, &activeFdSet);
					}
				}
				else
				{
					/* Data arriving on an already connected socket */
					if(readMessageFromClient(i) < 0)
					{
						close(i);
						FD_CLR(i, &activeFdSet);

					}
					else
					{
						sendMSG(i, 1);
					}
				}
			}
	}
}

