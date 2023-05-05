/*
This file contains the implementation of the functions declared in udp_transport.h. 
It should include the logic for connection setup and teardown, error check codes, 
sliding window mechanisms, and any other protocol stack functionality.
*/

#include "udp_transport.h"

void udt_send(Packet pkt)
{
    // TODO: Use a socket to send the packet using UDP
}
void rdt_rcv(Packet pkt)
{
    // TODO: Use a socket to receive the packet using UDP
}

void start_timer(int seqNum)
{

}

void stop_timer(int seqNum)
{

}

Packet make_pkt(int seqNum, char* data, int checksum)
{
    Packet pkt;
    pkt.seqNum = seqNum;
    pkt.data = data;
    pkt.checksum = checksum;
    return pkt;
}

void refuse_data(char* data)
{

}

void extract_data(Packet pkt, char* data)
{

}

void deliver_data(char* data)
{

}

void not_corrupt(Packet pkt)
{

}

void corrupt(Packet pkt)
{

}

void not_duplicate(Packet pkt)
{

}

void duplicate(Packet pkt)
{

}

void has_seq_num(Packet pkt, int seqNum)
{

}

void send(char* data)
{
    if(nextSeqNum < base + N){
        sndpkt[nextSeqNum] = make_pkt(nextSeqNum, data, checksum);
        udt_send(sndpkt[nextSeqNum]);
        start_timer(nextSeqNum);
        nextSeqNum++;
    }
    else
        refuse_data(data);
}