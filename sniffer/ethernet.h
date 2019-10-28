#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>

#define MAXBYTE2CAPTURE 2048

struct ether_header{
    u_int8_t ether_dhost[6];
    u_int8_t ether_shost[6];
    u_int16_t ether_type;
};

struct arp_header{
    u_int16_t    arp_hardware_type;
    u_int16_t    arp_protocol_type;
    u_int8_t    arp_hardware_length;
    u_int8_t    arp_protocol_length;
    u_int16_t    arp_operation_code;
    u_int8_t    arp_source_ethernet_address[6];
    u_int8_t    arp_source_ip_address[4];
    u_int8_t    arp_destination_ethernet_address[6];
    u_int8_t    arp_destination_ip_address[4];
};

struct ip_header{
    u_int8_t ver:4;
    u_int8_t ihl:4;
    u_int8_t tos;
    u_int16_t tLen;
    u_int16_t iden;
    u_int16_t fragOffset;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t checksum;
    struct in_addr sourceAddr;
    struct in_addr destAddr;
};

struct udp_header
{
    u_int16_t    udp_source_port;
    u_int16_t    udp_destination_port;
    u_int16_t    udp_length;
    u_int16_t    udp_checksum;
};

struct tcp_header
{
        u_int16_t    tcp_source_port;
        u_int16_t    tcp_destination_port;
        u_int32_t    tcp_acknowledgement;
        u_int32_t    tcp_ack;
        
#ifdef    WORDS_BIGENDIAN
        u_int8_t    tcp_offset:4,tcp_reserved:4;
#else
        u_int8_t    tcp_reserved:4,tcp_offset:4;
#endif

        u_int8_t    tcp_flags;
        u_int16_t    tcp_windows;
        u_int16_t    tcp_checksum;
        u_int16_t    tcp_urgent_pointer;
};


struct icmp_header
{
        u_int8_t    icmp_type;
        u_int8_t    icmp_code;
        u_int16_t    icmp_checksum;
        u_int16_t    icmp_id;
        u_int16_t    icmp_sequence;
};

void ip_protocol_packet_callback(const struct pcap_pkthdr *pkthdr, const u_char *packet);

void arp_protocol_packet_callback(const struct pcap_pkthdr *pkthdr, const u_char *packet);

void ethernet_protocol_packet_callback(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet);

void processPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet);

void tcp_protocol_packet_callback(const struct pcap_pkthdr*packet_header,const u_char*packet_content);

void udp_protocol_packet_callback(const struct pcap_pkthdr*packet_header,const u_char *packet_content);

void icmp_protocol_packet_callback(const struct pcap_pkthdr*packet_header,const u_char *packet_content);
