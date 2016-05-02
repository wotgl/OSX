#include <pcap.h> 
#include <string.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <fstream>
#define MAXBYTES2CAPTURE 2048 
#define SIZE_ETHERNET 14

void processPacket(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet);
void initSniffer(char *device);