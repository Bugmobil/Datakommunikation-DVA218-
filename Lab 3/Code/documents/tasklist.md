##  ✅ Initial Task List

1. Requirements for creating a protocol stack on top of UDP in Linux using C:

    - Familiarize yourself with the basics of C programming and socket programming in Linux.
    - Learn about UDP and its characteristics.
    - Understand the concepts of connection setup and teardown, error check codes, and sliding window mechanisms.

1. Develop a plan for implementing the protocol stack:

    - Decide on the overall structure of your client and server programs.
    - Outline the different components of your protocol stack, such as the three-way handshake, error check codes, and sliding window mechanisms.
    - Determine how you will handle lost, corrupt, and out-of-order packets using selective repeat or another sliding window protocol.

1. Begin implementing the protocol stack by creating the three-way handshake for connection setup and teardown:

    - Implement the client and server programs with socket programming.
    - Implement the SYN, SYN+ACK, and ACK packets required for the three-way handshake.
    - Implement the connection setup and teardown logic in your client and server programs.

1. Implement error check code calculation for each packet to ensure data integrity:

    - Choose an appropriate error check code algorithm, such as CRC or a checksum.
    - Implement the chosen algorithm in your client and server programs.
    - Calculate the error check code for each packet sent and verify it upon receipt.

1. Implement the chosen sliding window mechanism, starting with stop-and-wait and refining it to either go-back-N or selective repeat as needed:

    - Implement the basic stop-and-wait protocol for your client and server programs.
    - Test the stop-and-wait protocol to ensure it functions correctly.
    - Refine the stop-and-wait protocol to either go-back-N or selective repeat based on your requirements.

1. Test the protocol stack thoroughly to ensure that it functions properly and meets all requirements:

    - Test your protocol stack with various scenarios, such as lost, corrupt, and out-of-order packets.
    - Test the connection setup and teardown process.
    - Test the error check code and sliding window mechanisms.

1. Debug any issues that arise during testing and refine the implementation as needed:

    - Identify and resolve any issues discovered during testing.
    - Make any necessary refinements or optimizations to your protocol stack.

1. Document the protocol stack thoroughly, including how it was developed, how it works, and how to use it:

    - Write clear and concise documentation for your protocol stack.
    - Explain how each component of the stack works, such as the three-way handshake, error check codes, and sliding window mechanisms.
    - Provide instructions on how to use your protocol stack in a client-server environment.


## 📝 Task List
- [ ] Develop a user-friendly interface for the protocol stack, including clear instructions on how to use it and any necessary configuration options see [User guide](how_use.md)
- [ ] Implement packet retransmission in case of lost or corrupt packets, using the chosen sliding window mechanism
- [ ] Implement a mechanism to handle out-of-order packets by buffering them until the missing packets are received, and then delivering them in the correct order
- [ ] Implement congestion control mechanisms to prevent network congestion and ensure efficient use of network resources
- [ ] Optimize the protocol stack for performance, including minimizing latency and maximizing throughput
- [ ] Conduct stress testing and load testing to ensure that the 1. protocol stack can handle high volumes of traffic and maintain stability under heavy load
- [ ] Develop a comprehensive set of unit tests and integration tests to ensure that the protocol stack is reliable and free from bugs
- [ ] Implement logging and monitoring functionality to track network activity and identify any issues or anomalies
- [ ] Provide detailed documentation of the protocol stack, including troubleshooting guides.
- [ ] Research and stay up-to-date with the latest developments in networking protocols and technologies, and incorporate any relevant updates or improvements into the protocol stack.


## ✨Task execution

### **Logging and monitoring:**
We will also develop a logging and monitoring functionality to track network activity and identify any issues or anomalies. This will provide tools for analyzing and troubleshooting network performance.

To achieve this, We will implement the following steps:

1. Implement a logging system that records the details of each network transaction, including the source and destination IP addresses, port numbers, packet sequence numbers, and error check codes.

1. Monitor the network traffic in real-time to identify any issues or anomalies. This could include detecting packet loss, delay, and corruption, as well as identifying unusual patterns of network activity.

1. Provide tools for analyzing and troubleshooting network performance, such as graphical representations of network traffic, statistical analysis of packet loss and delay, and alerts for unusual network activity.

1. Integrate the logging and monitoring functionality with the protocol stack, so that it is automatically activated whenever the protocol is used.

1. Continuously monitor and analyze network performance, using the logging and monitoring tools to identify any issues and make recommendations for improving network performance.

By implementing a robust logging and monitoring system, We will ensure that network activity is tracked and analyzed in real-time, allowing issues to be identified and resolved quickly. This will improve network performance and reduce the risk of downtime or data loss.

### **Out-of-order:**
To implement the mechanism to handle out-of-order packets, We will need to keep track of the sequence number of each packet received. If a packet is received out of order, We will buffer it until the missing packets are received. Once the missing packets are received, We will deliver them in the correct order based on their sequence numbers.

To implement this mechanism, We will use a buffer to store the out-of-order packets. The buffer will be implemented as a linked list, with each node containing the packet data and its sequence number. When a packet is received, We will check its sequence number against the expected sequence number. If the packet's sequence number is greater than the expected sequence number, We will buffer it and send an acknowledgement for the last correctly received packet. If the packet's sequence number is less than the expected sequence number, We will discard it as a duplicate. If the packet's sequence number is equal to the expected sequence number, We will deliver it to the application layer and update the expected sequence number.

When a missing packet is received, We will search the buffer for the missing packet's sequence number. If the missing packet is found in the buffer, We will deliver it to the application layer and remove it from the buffer. We will then check the buffer for any other packets that may now be in order and deliver them as well.

To implement this mechanism, We will modify the receive function to include the buffer and the expected sequence number. We will also modify the send function to include the sequence number in each packet and to send acknowledgements for the last correctly received packet. Finally, We will modify the timeout function to retransmit packets that have not been acknowledged and to adjust the window size based on the number of unacknowledged packets.