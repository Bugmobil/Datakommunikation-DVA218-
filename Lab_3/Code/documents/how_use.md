# Welcome
### Our protocol stack provides reliable data transfer by implementing a sliding window mechanism, error check codes, and connection setup/teardown using a three-way handshake.

**To use our protocol stack, follow these steps:**

1. Identify the IP address and port number of the destination host.
1. Configure the protocol stack by setting the following parameters:
    - Maximum segment size (MSS): the maximum amount of data that can be sent in a single packet.
    - Window size: the number of packets that can be sent before waiting for an acknowledgement.
    - Timeout interval: the amount of time to wait before retransmitting a packet.
    - Error check code algorithm: choose either CRC or checksum.
1. Establish a connection using the three-way handshake:
    - The client sends a SYN packet to the server.
    - The server responds with a SYN-ACK packet.
    - The client sends an ACK packet to confirm the connection.
1. Send data using the sliding window mechanism:
    - The client sends packets of data to the server.
    - The server sends an acknowledgement (ACK) for each packet received.
    - If a packet is lost or corrupted, the sender will retransmit it after a timeout interval.
    - If the sender receives three duplicate ACKs for the same packet, it assumes the packet was lost and retransmits it.
1. Terminate the connection using the three-way handshake:
    - The client sends a FIN packet to the server.
    - The server responds with an ACK packet.
    - The server sends a FIN packet to the client.
    - The client responds with an ACK packet to confirm the teardown of the connection.

By following these steps, you can use our protocol stack to reliably transfer data over UDP.