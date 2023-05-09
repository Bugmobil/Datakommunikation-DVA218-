# Lab 3a



| Majid Azizi mai20018 | Andreas Pearson apn21017|
| - | - |

### Introduction
In this report, we will present our plan to implement the Sliding Window Protocol for our communication system. We will describe the three-way handshake, sliding window as well as the teardown which contain error handling.  They will be described using finite state machines.

### Three-way handshake
The three-way handshake is used to establish a reliable connection between the client and the server. We will use two state machines to present the process, one for the client and one for the server.

#### State Machine for Client:
![image](https://cdn.discordapp.com/attachments/889444874502438952/1100365513483354122/Client_Setup.png)

#### State Machine for Server: 
![image](https://cdn.discordapp.com/attachments/889444874502438952/1100365514112503828/Server_Setup.png)

The last timeout for the client is to make sure to wait and give the server a chance to resend an SYN & ACK in case it did not reach the client.

### Teardown
Teardown is used to close the connection between the client and the server in a proper and orderly manner.  We will use two state machines to represent the process, one for the client and one for the server.

#### State Machine for Client:
![image](https://cdn.discordapp.com/attachments/889444874502438952/1100365513827307571/Client_Teardown.png)
#### State Machine for Server:
![image](https://cdn.discordapp.com/attachments/889444874502438952/1100365514305437786/Server_Teardown.png)
The last timeout in the client, like the setup, is for the client to make sure to wait and give the server a chance to resend a FIN in case it did not reach the client.

### Sliding Window Protocol
The Sliding Window Protocol is used to ensure that the data is transmitted correctly and efficiently. We will use two state machines to represent the process, one for the sender and one for the receiver.

![image](https://cdn.discordapp.com/attachments/889444874502438952/1100367328006389830/SlidingWindowSR.png)
