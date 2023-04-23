
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
- Used to efficiently manage data flow and control
- Two main types:
   1. Go-Back-N
      - Sender maintains a window of packets to send
      - Receiver sends ACK for the highest in-order packet received
      - If a packet is lost or corrupted, sender retransmits from the lost packet and continues
   2. Selective Repeat
      - Sender maintains a window of packets to send
      - Receiver sends ACK for each received packet, regardless of order
      - Sender retransmits only lost or corrupted packets

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
