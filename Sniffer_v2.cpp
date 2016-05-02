#include "Sniffer_v2.h"

/* processPacket(): Callback function called by pcap_loop() everytime a packet */
/* arrives to the network card. This function prints the captured raw data in  */
/* hexadecimal.                                                                */
void processPacket(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet){ 
    int i=0, *counter = (int *)arg; 
    // printf("Packet Count: %d\n", ++(*counter)); 
    printf("Received Packet Size: %d\n", pkthdr->len); 
    printf("Payload:\n"); 
    for (i = 0; i < pkthdr->len; i++){ 
        if (isprint(packet[i])) /* If it is a printable character, print it */
            printf("%c ", packet[i]); 
        else 
            printf(". "); 

        if((i%32 == 0 && i != 0) || i == pkthdr->len - 1) 
            printf("\n"); 
    } 
    return; 
} 

void initSniffer(char *device){
    int count = 0;
    pcap_t *descr = NULL; 
    char errbuf[PCAP_ERRBUF_SIZE]; 
    memset(errbuf,0,PCAP_ERRBUF_SIZE); 

    printf("Opening device %s\n", device); 
    /* Open device in promiscuous mode */ 
    if ((descr = pcap_open_live(device, MAXBYTES2CAPTURE, 1,  1000, errbuf)) == NULL){
        fprintf(stderr, "ERROR: %s\n", errbuf);
    }

    std::ofstream outfile ("traffic.pcap",std::ofstream::binary);
    /* Loop forever*/ 
    if (pcap_loop(descr, -1, processPacket, (u_char *)&count) == -1){
        fprintf(stderr, "ERROR: %s\n", pcap_geterr(descr) );
    }
}
