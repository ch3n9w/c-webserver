#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include "ethernet.h"


int main(int argc, char *argv[]){
    /* if(argc != 2){ */
	/* printf("USAGE: ./output <interface>\n"); */
	/* exit(1); */
    /* } */
    /* if((descr=pcap_open_live(argv[1], MAXBYTE2CAPTURE, 1, 512, errbuf)) != -1) */
	/* pcap_loop(descr, 10, ethernet_protocol_packet_callback, argv[1]); */

    int i=0;
    pcap_t *descr = NULL;
    pcap_if_t *interfaces, *device;
    char errbuf[PCAP_ERRBUF_SIZE];
    if(pcap_findalldevs(&interfaces, errbuf) == -1){
	fprintf(stderr, "can not find interfaces");
	return -1;
    }

    device = interfaces;
    descr = pcap_open_live(device->name, MAXBYTE2CAPTURE, 1, 512, errbuf);

    pcap_loop(descr, 10, ethernet_protocol_packet_callback, (u_char *)device->name);
    pcap_close(descr);
    
}
