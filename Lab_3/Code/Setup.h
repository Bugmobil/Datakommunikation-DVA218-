#include "Utils.h"

void ClientSetup(int fd, struct sockaddr* addr, socklen_t addrLen);
void ServerSetup(int fd, struct sockaddr* destAddr, socklen_t* addrLen);

void SendSYN(int fd, struct sockaddr* destAddr, socklen_t addrLen);
void SendACK(int fd, struct sockaddr* destAddr, socklen_t addrLen);
void SendSYNACK(int fd, struct sockaddr* destAddr, socklen_t addrLen);

int ReceiveSYN(int fd, struct sockaddr* src_addr, socklen_t* addrLen);
int ReceiveACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen);
int ReceiveSYNACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen);

