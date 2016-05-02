#include <pcap.h> 
#include <string.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <fstream>
#define MAXBYTES2CAPTURE 2048 
#define SIZE_ETHERNET 14

void initSniffer(char *device);

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void print_payload(const u_char *payload, int len);

void print_hex_ascii_line(const u_char *payload, int len, int offset);

void print_current_time();