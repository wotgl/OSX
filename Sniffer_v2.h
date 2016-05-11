#include <pcap.h> 
#include <string.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <string.h>
#define MAXBYTES2CAPTURE 2048 
#define SIZE_ETHERNET 14

void initSniffer(char *device);

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void print_payload(const u_char *payload, int len);

void print_hex_ascii_line(const u_char *payload, int len, int offset);

char* get_current_time();

void check_file();

void tar_log_file(char *filename);

int send_to_server(char *data, int data_size);