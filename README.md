# Project 3 CS 5153

### The VMs were not able to be ran, due to computing power restrictions. Exploits developed using cheap vps and local box.
### So values/addrs will differ from those in the VM env

- For Task 1: please use netwox to perform SYN flooding on a telnet connection from a user to a server,
where SYN cookies is off on the server; You do not need to test the case when SYN cookies is on.
- For Task 2: please use scapy to perform TCP Reset on a telnet connection from a user to a server.
Please use both manual and automated methods to decide the sequence number of a spoofed RST
packet as we mentioned in our lectures. You do not need to perform the TCP Reset with netwox or
test the case with a ssh connection.
– For Task 4: please use scapy to perform TCP Hijacking on a telnet connection from a user to a server
such that an attacker can print out the content of a secret file (/home/seed/secret.txt) from the
server; please include your ucid and a fake password as the content of this secret file. You do not need
to perform TCP Hijacking with netwox
- Optional Task, Create a reverse shell using scapy