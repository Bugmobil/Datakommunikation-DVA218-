
# Assignment Plan

1. **Research Concepts**
   - Connection setup and teardown
   - Error check codes
   - Sliding window mechanisms
   - Handling packet issues

2. **Write Theoretical Part**
   a. Describe your solution, including:
      - State machines for connection setup, sliding window protocol, and teardown (for both sender and receiver)
   b. Explain how you'll handle lost, corrupt, and out-of-order packets

3. **Implement Client and Server**
   a. Create the protocol stack on top of UDP, including:
      - Connection setup/teardown
      - Error check codes
      - Sliding window mechanisms
   b. Implement three-way handshake for connection setup and teardown, handling lost, corrupt, and out-of-order packets
   c. Implement error check code calculation for each packet
   d. Implement sliding window mechanism:
      - Start with a stop-and-wait protocol
      - Refine it to either "go-back-N" or "selective repeat"

4. **Test and Demonstrate Solution**
   a. Simulate lost packets, corrupt packets, packets out-of-order, etc.
   b. Show that your solution can handle these issues correctly and efficiently

5. **Document Solution in a Report**
   - Follow the criteria provided earlier

# Reliable Transport Protocol Guide

## 1. Connection Setup and Teardown

### Connection Setup
- Establish a connection between sender and receiver
- A common method is the three-way handshake:
   1. Sender (S) -> Receiver (R): SYN packet
   2. Receiver (R) -> Sender (S): SYN+ACK packet
   3. Sender (S) -> Receiver (R): ACK packet

### Connection Teardown
- Close the connection between sender and receiver
- A common method is a four-way handshake:
   1. Sender (S) -> Receiver (R): FIN packet
   2. Receiver (R) -> Sender (S): ACK packet
   3. Receiver (R) -> Sender (S): FIN packet
   4. Sender (S) -> Receiver (R): ACK packet

## 2. Error Check Codes
- Used to detect data corruption during transmission
- Common methods include:
   - Checksum
   - Cyclic Redundancy Check (CRC)

## 3. Sliding Window Mechanisms
   Used to efficiently manage data flow and control
### **Go-Back-N**
  1. Define the window size (N): Choose a suitable window size for your protocol. The window size determines the maximum number of unacknowledged packets that can be in transit at any given time.

  2. Sender side:

     - Maintain two variables: **'base'** (oldest unacknowledged packet) and **'nextseqnum'** (next packet to be sent).
     - Send packets within the window: If **'nextseqnum'** < **'base'** + N, send the packet with the sequence number **'nextseqnum'** and increment **'nextseqnum'**.
     - Set a timer for the oldest unacknowledged packet (packet with sequence number **'base'**).
     - If the timer expires, retransmit all unacknowledged packets and restart the timer for the oldest unacknowledged packet.

  3. Receiver side:

      - Maintain a single variable: **'expectedseqnum'** (next expected packet).
      - When a packet arrives, check its sequence number:
      - If it matches **'expectedseqnum'**, send an ACK for the packet, and increment **'expectedseqnum'**.
      - If it doesn't match, discard the packet and resend the ACK for the last correctly received packet (with sequence number **'expectedseqnum - 1'**).
  4. Sender side (processing ACKs):

      - When an ACK arrives, update the **'base'** variable to be the next expected sequence number (**'ACK_sequence_number + 1'**).
      - If there are unacknowledged packets left (i.e., **'base'** < **'nextseqnum'**), restart the timer for the oldest unacknowledged packet.
      - If all packets have been acknowledged, stop the timer.
      - 
### **Selective Repeat**
      
   1. Define the window size (N): Choose a suitable window size for your protocol. The window size should be even, and the sequence number space should be at least twice the window size.

   2. Sender side:

      - Maintain two variables: **'base'** (oldest unacknowledged packet) and nextseqnum (next packet to be sent).
      - Send packets within the window: If **'nextseqnum'** < **'base'** + N, send the packet with the sequence number **'nextseqnum'** and increment **'nextseqnum'**.
      - Set individual timers for each packet in the window. If a timer expires, retransmit the corresponding packet and restart its timer.
   
   3. Receiver side:

      - Maintain a buffer for out-of-order packets and a variable **'expectedseqnum'** (next expected packet).
      - When a packet arrives, send an ACK for the packet immediately, regardless of its sequence number.
      - If the packet has the expected sequence number, increment **'expectedseqnum'** and deliver any consecutive buffered packets.

   4. Sender side (processing ACKs):

      - When an ACK arrives, mark the corresponding packet as acknowledged.
      - If the ACK is for the oldest unacknowledged packet (packet with sequence number **'base'**), update the **'base'** variable to the next unacknowledged sequence number and cancel the timer for the acknowledged packet.

## 4. Handling Packet Issues
- Deal with lost, corrupt, and out-of-order packets in transmission
- Solutions should consider these issues during:
   - Connection setup and teardown
   - Data transfer
- Techniques include:
   - Retransmission upon timeout or negative ACK
   - Buffering and reordering out-of-order packets


# State Machines
- Used to model the behavior of a system

| Moore Machine | Mealy Machine | 
--|-|
| Output depends only upon the present state. |	Output depends on the present state as well as present input. |
|Moore machine also places its output on the transition.	|Mealy Machine places its output on the transition.|
|More states are required. |	Less number of states are required.  |
|There is less hardware requirement for circuit implementation. |	There is more hardware requirement for circuit implementation.  |
| They react slower to inputs(One clock cycle later). |	They react faster to inputs.  |
|Synchronous output and state generation. |	Asynchronous output generation.  |
|Output is placed on states.| 	Output is placed on transitions.|  
|Easy to design. 	| It is difficult to design.| 



