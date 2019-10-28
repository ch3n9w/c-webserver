#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include "ethernet.h"


void tcp_protocol_packet_callback(const struct pcap_pkthdr* pkthdr,const u_char* packet){
    struct tcp_header *tcp_protocol;
    u_char flags;
    int header_length;
    u_short source_port;
    u_short destination_port;
    u_short windows;
    u_short urgent_pointer;
    u_int    sequence;
    u_int    acknowledgement;
    u_int16_t    checksum;

    tcp_protocol = (struct tcp_header*)(packet+14+20);
    source_port = ntohs(tcp_protocol->tcp_source_port);
    destination_port = ntohs(tcp_protocol->tcp_destination_port);
    header_length = tcp_protocol->tcp_offset*4;
    sequence = ntohl(tcp_protocol->tcp_acknowledgement);
    acknowledgement = ntohl(tcp_protocol->tcp_ack);
    windows = ntohs(tcp_protocol->tcp_windows);
    urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);
    flags = tcp_protocol->tcp_flags;
    checksum = ntohs(tcp_protocol->tcp_checksum);
    printf("----------------  TCP Protocol  (Transport Layer)  ------------------\n");

    printf("Source Port: %d\n",source_port);
    printf("Destination Port: %d\n",destination_port);

    switch(destination_port)
    {
	case 80:
	    printf("HTTP protocol\n");
	    break;
	case 21:
	    printf("FTP  protocol\n");
	    break;
	case 23:
	    printf("TELNET protocol\n");
	    break;
	case 25:
	    printf("SMTP  protocol\n");
	    break;
	case 110:
	    printf("POP3 protocol\n");
	    break;
	default:
	    break;
    }

    printf("Sequence Number:%u\n",sequence);
    printf("Acknowledgement Number:%u\n",acknowledgement);
    printf("Header Length:%d\n",header_length);
    printf("Reserved: %d\n",tcp_protocol->tcp_reserved);
    printf("Flags: ");

    if(flags & 0x08)
	printf("PSH ");
    if(flags & 0x10)
	printf("ACK ");
    if(flags & 0x02)
	printf("SYN ");
    if(flags & 0x20)
	printf("URG ");
    if(flags & 0x01)
	printf("FIN ");
    if(flags & 0x04)
	printf("RST ");

    printf("\n");
    printf("Windows Size: %d\n",windows);
    printf("Checksum: %d\n",checksum);

    printf("Urgent pointer: %d\n",urgent_pointer);

}

void udp_protocol_packet_callback(const struct pcap_pkthdr*packet_header,const u_char *packet_content){
    struct udp_header *udp_protocol;
    u_short source_port;
    u_short destination_port;
    u_short    length;

    udp_protocol = (struct udp_header *)(packet_content+14+20);
    source_port = ntohs(udp_protocol->udp_source_port);
    destination_port = ntohs(udp_protocol->udp_destination_port);
    length = ntohs(udp_protocol -> udp_length);

    printf("-------------------   UDP Protocol (Transport Layer)  ----------------------\n");

    printf("Source port: %d\n",source_port);
    printf("Destination port: %d\n",destination_port);

    switch(destination_port)
    {
	case 138:
	    printf("NETBIOS Datagram Service\n");
	    break;
	case 137:
	    printf("NETBIOS Name Service\n");
	    break;
	case 139:
	    printf("NETBIOS session service\n");
	    break;
	case 53:
	    printf("name-domain service \n");
	    break;
	default:
	    break;
    }

    printf("Length: %d\n",length);
    printf("Checksum: %d\n",ntohs(udp_protocol->udp_checksum));
}

void icmp_protocol_packet_callback(const struct pcap_pkthdr*packet_header,const u_char *packet_content){
    struct icmp_header *icmp_protocol;
    icmp_protocol = (struct icmp_header *)(packet_content + 14 + 20);

    printf("------------------- ICMP Protocol  (Transport Layer) ---------------------\n");
    printf("ICMP Type: %d\n",icmp_protocol->icmp_type);

    switch(icmp_protocol->icmp_type)
    {
	case 8:
	    printf("ICMP Echo Request Protocol \n");
	    printf("ICMP Code: %d\n",icmp_protocol->icmp_code);
	    printf("Identifier: %d\n",icmp_protocol->icmp_id);
	    printf("Sequence Number: %d\n",icmp_protocol->icmp_sequence);

	    break;
	case 0:

	    printf("ICMP Echo Reply Protocol \n");
	    printf("ICMP Code: %d\n",icmp_protocol->icmp_code);
	    printf("Identifier: %d\n",icmp_protocol->icmp_id);
	    printf("Sequence Number: %d\n",icmp_protocol->icmp_sequence);

	    break;
	default:
	    break;
    }

    printf("ICMP Checksum: %d\n",ntohs(icmp_protocol->icmp_checksum));
}

void ip_protocol_packet_callback(const struct pcap_pkthdr *pkthdr, const u_char *packet){
    struct ip_header *ip_protocol;

    u_int header_length;
    u_int offset;
    u_char tos;
    u_int16_t checksum;
    ip_protocol = (struct ip_header*)(packet+14);

    checksum = ntohs(ip_protocol->checksum);
    header_length = ip_protocol->tLen;
    offset = ntohs(ip_protocol->fragOffset);
    tos = ip_protocol->tos;
    printf("-----------------  IP  Protocol  (Network  Layer)  ------------------\n");
    printf("IP Version: %d\n",ip_protocol->ver);
    printf("Header length: %d\n",header_length);
    printf("TOS: %d\n",tos);
    printf("Total length: %d\n",ntohs(ip_protocol->tLen));
    printf("Identification: %d\n",ntohs(ip_protocol->iden));
    printf("Offset: %d\n",(offset & 0x1fff) * 8);
    printf("TTL: %d\n",ip_protocol->ttl);
    printf("Protocol: %d\n",ip_protocol->protocol);

    switch(ip_protocol->protocol)
    {
	case 6:
	    printf("The Transport Layer Protocol is TCP\n");
	    break;
	case 17:
	    printf("The Transport Layer Protocol is UDP\n");
	    break;
	case 1:
	    printf("The Transport Layer Protocol is ICMP\n");
	    break;
	default:
	    break;
    }
    printf("Header checksum: %d\n",checksum);
    printf("Source address: %s\n",inet_ntoa(ip_protocol->sourceAddr));
    printf("Destination address: %s\n",inet_ntoa(ip_protocol->destAddr));

    switch(ip_protocol->protocol)
    {
	case 6:
	    tcp_protocol_packet_callback(pkthdr,packet);
	    break;
	case 17:
	    udp_protocol_packet_callback(pkthdr,packet);
	    break;
	case 1:
	    icmp_protocol_packet_callback(pkthdr,packet);
	    break;
	default:
	    break;
    }
}

void arp_protocol_packet_callback(const struct pcap_pkthdr *pkthdr, const u_char *packet){
    struct arp_header *arp_protocol;
    u_short    protocol_type;
    u_short    hardware_type;
    u_short    operation_code;
    u_char    *mac_string;
    struct in_addr source_ip_address;
    struct in_addr destination_ip_address;

    u_char hardware_length;
    u_char protocol_length;

    printf("--------------------   ARP  Protocol (Network Layer)   --------------------\n");

    arp_protocol = (struct arp_header *)(packet+14);

    hardware_type = ntohs(arp_protocol->arp_hardware_type);
    protocol_type = ntohs(arp_protocol->arp_protocol_type);
    operation_code = ntohs(arp_protocol->arp_operation_code);
    hardware_length = arp_protocol->arp_hardware_length;
    protocol_length = arp_protocol->arp_protocol_length;

    printf("ARP Hardware Type: %d\n",hardware_type);
    printf("ARP Protocol Type: %d\n",protocol_type);
    printf("ARP Hardware Length: %d\n",hardware_length);
    printf("ARP Protocol Length: %d\n",protocol_length);
    printf("ARP Operation: %d\n",operation_code);

    switch(operation_code)
    {
	case 1:
	    printf("ARP Request Protocol\n");
	    break;
	case 2:
	    printf("ARP Reply Protocol\n");
	    break;
	case 3:
	    printf("RARP Request Protocol\n");
	    break;
	case 4:
	    printf("RARP Reply Protocol\n");
	    break;
	default:
	    break;
    }
    printf("Ethernet Source Address is :\n");
    mac_string = arp_protocol->arp_source_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",*mac_string,*(mac_string+1),
	    *(mac_string+2),*(mac_string+3),
	    *(mac_string+4),*(mac_string+5));
    memcpy((void*)&source_ip_address,(void*)&arp_protocol->arp_source_ip_address,sizeof(struct in_addr));
    printf("Source IP Address: %s\n",inet_ntoa(source_ip_address));
    printf("Ethernet Destination Address is: \n");
    mac_string = arp_protocol->arp_destination_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",*mac_string,*(mac_string+1),
	    *(mac_string+2),*(mac_string+3),
	    *(mac_string+4),*(mac_string+5));

    memcpy((void *)&destination_ip_address,(void*)&arp_protocol->arp_destination_ip_address,sizeof(struct in_addr));

    printf("Destination IP Address: %s\n",inet_ntoa(destination_ip_address));
}


void ethernet_protocol_packet_callback(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet){
    u_short ethernet_type;
    struct ether_header * ethernet_protocol;
    u_char *mac_string;
    static int packet_number = 1;
    char *net_interface = (char *)arg;
    printf("---------------------*****----------------------\n");
    printf("the %d packet is captured. \n",packet_number);
    printf("Capture a Packet from net_interface: \n");
    printf(">>>    %s\n", net_interface);
    printf("Capture Time is: \n");
    printf(">>>    %s\n", ctime((const time_t*)&pkthdr->ts.tv_sec));
    printf("Packet length is:\n");
    printf(">>>    %d\n",pkthdr->len);
    ethernet_protocol = (struct ether_header*)packet;
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    printf(">>>    %04x\n",ethernet_type);

    switch(ethernet_type){
	case 0x0800:
	    printf("The network layer is IP protocol\n");
	    break;
	case 0x0806:
	    printf("The network layer is ARP protocol\n");
	    break;
	case 0x8035:
	    printf("The network layer is RARP protocl\n");
	    break;
	default:
	    break;
    }

    printf("Mac Source Address is :\n");
    mac_string = ethernet_protocol->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string+1), *(mac_string+2), *(mac_string+3), *(mac_string+4), *(mac_string+5));
    printf("Mac Destination Address is :\n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string+1), *(mac_string+2), *(mac_string+3), *(mac_string+4), *(mac_string+5));
    printf("---------------------*****----------------------\n");
    
    switch(ethernet_type){
	case 0x0800:
	    ip_protocol_packet_callback(pkthdr, packet);
	    break;
	case 0x0806:
	    arp_protocol_packet_callback(pkthdr, packet);
	    break;
	case 0x8035:
	    printf("The network layer is RARP protocl\n");
	    break;
	default:
	    break;
    }
    packet_number ++;
}


void processPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet){
    int i=0, *counter = (int *)arg;
    printf("Packet Count :%d\n", ++(*counter));
    printf("Received Packet Size: %d\n", pkthdr->len);
    printf("Payload:\n");

    for(i = 0; i< pkthdr->len; i++){
	if(isprint(packet[i]))
	    printf("%c ", packet[i]);
	else
	    printf(". ");

	if ((i%16==0 && i != 0) || i==pkthdr->len-1 )
	    printf("\n");
    }
    return;
}
