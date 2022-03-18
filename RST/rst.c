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
//#include <linux/if_ether.h>  
#include <netinet/if_ether.h> //MacOS
#include <unistd.h>  

int main(){
    struct sockaddr addr;
    int sock, n;
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