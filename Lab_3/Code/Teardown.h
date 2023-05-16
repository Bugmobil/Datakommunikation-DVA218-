#include "Setup.h"

void ClientTeardown(int fd, const struct sockaddr *addr, socklen_t addrLen);
void ServerTeardown(int fd, const struct sockaddr* destAddr, socklen_t addrLen);

void SendFIN(int fd, const struct sockaddr* destAddr, socklen_t addrLen);
void SendFINACK(int fd, const struct sockaddr* destAddr, socklen_t addrLen);

int ReceiveFIN(int fd, struct sockaddr* src_addr, socklen_t* addrLen);
int ReceiveFINACK(int fd, struct sockaddr* src_addr, socklen_t* addrLen);