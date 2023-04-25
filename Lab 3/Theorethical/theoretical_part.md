
# Connection Setup and Teardown

## Connection Setup
- Implement a protocol for establishing connections between sender (S) and receiver (R)
- Use a three-way handshake similar to TCP:
   1. Sender (S) -> Receiver (R): SYN packet
   2. Receiver (R) -> Sender (S): SYN+ACK packet
   3. Sender (S) -> Receiver (R): ACK packet

## Connection Teardown
- Implement a protocol for closing connections between sender (S) and receiver (R)
- Use a process similar to TCP with a FIN flag
- Handle lost, corrupt, and out-of-order packets during connection teardown

# Error Check Code
- Include an error check code in each UDP packet sent
- Ensure the packet hasn't been modified
- Calculate the error check code using both the packet header and its data (excluding the UDP header information)

# Sliding Window Mechanism
- Implement a more efficient protocol than stop-and-wait
- Choose between "go-back-N" or "selective repeat" sliding window mechanisms
- Start with a stop-and-wait protocol and refine it into a sliding window protocol

# Handling Packet Issues
- Handle lost, corrupt, and out-of-order packets during:
   - Data transmission
   - Connection setup
   - Connection teardown




