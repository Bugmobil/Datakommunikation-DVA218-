/*
This file contains the implementation of the utility functions declared in utils.h.
*/

#include "Utils.h"

void InitPacket(Packet* packet)
{
    packet->ACK = 0;
    packet->SYN = 0;
    packet->FIN = 0;
    packet->data[0] = '\0';
    packet->dataSize = 0;
    packet->seqNum = 0;
    packet->timestamp = 0;
    packet->checksum = 0;
}

void Serialize(char* buffer, Packet packet)
{
    uint16_t flags = 0;
    uint32_t dataSize = htonl(packet.dataSize);
    uint32_t seqNum = htonl(packet.seqNum);
    uint32_t timestamp = htonl(packet.timestamp);
    uint32_t checksum = htonl(packet.checksum);
    
    if (packet.ACK) flags |= (1 << 0);
    if (packet.SYN) flags |= (1 << 1);
    if (packet.FIN) flags |= (1 << 2);

    memcpy(buffer, &flags, sizeof(uint16_t));
    memcpy(buffer + sizeof(uint16_t) + sizeof(uint32_t), &packet.data, packet.dataSize);
    memcpy(buffer + messageLength + sizeof(uint16_t), &seqNum, sizeof(uint32_t));
    memcpy(buffer + messageLength + sizeof(uint16_t) + sizeof(uint32_t), &dataSize, sizeof(uint32_t));
    memcpy(buffer + messageLength + sizeof(uint16_t) + 2 * sizeof(uint32_t), &timestamp, sizeof(uint32_t));
    memcpy(buffer + messageLength + sizeof(uint16_t) + 3 * sizeof(uint32_t), &checksum, sizeof(uint32_t));
}

void Deserialize(char* buffer, Packet* packet)
{
    uint16_t flags;
    uint32_t dataSize;
    uint32_t seqNum;
    uint32_t timestamp;
    uint32_t checksum;
}