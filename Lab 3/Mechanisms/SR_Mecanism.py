from diagrams import Diagram, Edge
from diagrams.generic.network import Switch, Router

with Diagram("Selective Repeat Sliding Window Protocol", show=False):
    sender = Router("Sender")
    receiver = Router("Receiver")

    # Sender Side
    sender_base = Switch("base")
    sender_nextseqnum = Switch("nextseqnum")
    sender_timer = Switch("Timer")

    # Receiver Side
    receiver_buffer = Switch("Buffer")
    receiver_expectedseqnum = Switch("expectedseqnum")

    sender >> Edge(label="Send packets within the window") >> sender_nextseqnum >> sender_base
    sender_base >> Edge(label="Set individual timers") >> sender_timer
    sender_timer >> Edge(label="Timer expires, retransmit") >> sender

    # Communication
    sender >> Edge(label="Transmit packets") >> receiver
    receiver >> Edge(label="Send ACK") >> sender

    receiver >> Edge(label="Receive packets") >> receiver_expectedseqnum >> receiver_buffer
    receiver_buffer >> Edge(label="Deliver consecutive buffered packets") >> receiver_expectedseqnum

    # Processing ACKs
    sender >> Edge(label="Process ACKs") >> sender_base
