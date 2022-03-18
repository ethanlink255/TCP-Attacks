#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h> //prob unnecessary
#include <linux/if_ether.h>  
//#include <netinet/if_ether.h> //MacOS
#include <unistd.h>  
#include <stdlib.h>


void handlePacket(char *buf, int s){
    struct iphdr *ip = (struct iphdr*)(buf + sizeof(struct ethhdr));
    if(ip->protocol == 6){
        unsigned short hdrlen;
        hdrlen = ip->ihl * 4;

        struct tcphdr *tcph = (struct tcphdr*)(buf + hdrlen + sizeof(struct ethhdr));

        int seq_num = tcph->seq;

        int sock;

        if((sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {
            perror("Failed to Create Socket");
            exit(4);
        }

        char datagram[4096];
        memset(datagram, 0, 4096);

        struct iphdr *ret = (struct iphdr*) datagram;
        
        struct tcphdr *spoof = (struct tchhdr*) (datagram + sizeof(stuct ip));
        struct sockaddr_in sin;
        struct pseudo_header psh;

        ret->ihl = 5;
        ret->version = 4;
        ret->tos = 0;
        ret->tot_len = sizeof(strcut iphdr) + sizeof(struct tcphdr);
        ret->id = htonl(28313); //prob doesn't matter
        ret->frag_off = 0;
        ret->ttl = 255; //max
        ret->protocol = IPPROTO_TCP;
        ret->check = 0;
        ret->saddr = ip->daddr;
        ret->daddr = ip->saddr;


        struct iphdr spoof;
        spoof.dest = inet_pton(ip->src);
        spoof.src = inet_pton(ip->dest);
    


    }
}

int main(){
    struct sockaddr addr;
    int sock;
    unsigned char *buffer = malloc(65536);
    char *eth_name = "";

    struct ifreq ethreq;
 
    if((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) < 0)){
        perror("Socket Creation Failed");
        return 1;
    }

    strncpy(ethreq.ifr_name, eth_name, IFNAMSIZ); //creates a control stucture that will be used to enable prom. mode
    if(ioctl(sock, SIOCGIFFLAGS, &ethreq) == -1){ //copies flags into ethreq
        perror("Could not get flags");
        close(sock);
        return 2;
    }

    ethreq.ifr_flags |= IFF_PROMISC; //enable promiscuous flag
    if (ioctl(sock, SIOCSIFFLAGS, &ethreq) == -1){ //set flags
        perror("Could not set flags");
        close(sock);
        return 3;
    }

    int n;

    while (1) {
        n = recvfrom(sock, buffer, 65536, 0, NULL, NULL);
        if(!(n < 0)){
            handlePacket(buffer, n);
        }
    }
    
    ethreq.ifr_flags ^= IFF_PROMISC;
    ioctl(sock, SIOCSIFFLAGS, &ethreq);
    close(sock);
    return 0;






}