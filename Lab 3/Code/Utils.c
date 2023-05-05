#include "Utils.h"

void initPacket(Packet* packet)
{
    packet->sequenceNumber = 0;
    packet->data[0] = '\0';
    packet->dataSize = 0;
    packet->ACK = 0;
    packet->SYN = 0;
    packet->FIN = 0;
    packet->timestamp = 0;
    packet->checksum = 0;
}
/*
This file contains the implementation of the utility functions declared in utils.h.
*/
