#include "Utils.h"

void ClientSetup(int fd, struct sockaddr *addr, socklen_t addrLen);
void ServerSetup(int fd, const struct sockaddr* destAddr, socklen_t addrLen);

void SendSYN(int fd, const struct sockaddr* destAddr, socklen_t addrLen);
void SendACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen);
void SendSYNACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen);

int ReceiveSYN(int fd, struct sockaddr* src_addr, socklen_t* addrLen);
int ReceiveACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen);
int ReceiveSYNACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen);

