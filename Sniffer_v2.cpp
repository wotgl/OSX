#include "Sniffer_v2.h"

#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN  6

char FILENAME[1024];
FILE* FILE_PTR;
size_t MAX_PACKETS_TO_WRITE = 20;


/* Ethernet header */
struct sniff_ethernet {
        u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
        u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
        u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
        u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off;                 /* fragment offset field */
        #define IP_RF 0x8000            /* reserved fragment flag */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};


char* get_current_time() {
    int buffer_size = 100;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char *buff = (char *)malloc(sizeof(char) * buffer_size);
    memset(buff, 0, buffer_size);
    snprintf(buff, sizeof(buff), "%d-%d-%d_%d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return buff;
}

void check_file() {
    static size_t count = 0;

    count++;
    if (count > MAX_PACKETS_TO_WRITE) {
        count = 0;
        fclose(FILE_PTR);

        std::thread tar(tar_log_file, FILENAME);
        tar.join();
        
        char *time_filename = get_current_time();
        snprintf(FILENAME, sizeof(FILENAME), "%s.txt", time_filename);
        free(time_filename);
        FILE_PTR = fopen(FILENAME, "wb");
        if (FILE_PTR == NULL) {
            fprintf(stderr, "Error opening file!\n");
            exit(EXIT_FAILURE);
        }
    }
}

void tar_log_file(char *filename) {
    char command[1024], tar_filename[1024];

    snprintf(command, sizeof(command), "tar czf %s.tar.gz %s", filename, filename);
    system(command);

    // Tar filename
    snprintf(tar_filename, sizeof(tar_filename), "%s.tar.gz", filename);

    send_to_server(tar_filename);
}

int send_to_server(char *filename) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  portno = 12345;

  // Create a socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) return -1;

  server = gethostbyname("127.0.0.1");

  if (server == NULL) return -1;

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("[error]\tConnecting to server\n");
    return -1;
  }

  // Get size of tar file
  struct stat st;
  stat(filename, &st);
  int size = st.st_size;
  fcntl(sockfd, F_SETFL, O_NONBLOCK);

  // File to buffer
  int tmp = 0;
  unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * size);
  memset(buffer, 0, size);
  FILE *f = fopen(filename, "rb");
  for (int i = 0; i < size; ++i) {
    unsigned char c = getc(f);
    buffer[i] = c;
    if (c == 0) {
      n = write(sockfd, buffer + tmp, strlen((char*)buffer + tmp) + 1);
      tmp = i + 1;
    }
  }
  fclose(f);
  free(buffer);

  if (n < 0) return -1;

  return 0;
}

/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void print_hex_ascii_line(const u_char *payload, int len, int offset) {
    int i;
    int gap;
    const u_char *ch;

    /* offset */
    fprintf(FILE_PTR, "%05d   ", offset);
    
    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        fprintf(FILE_PTR, "%02x ", *ch);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            fprintf(FILE_PTR, " ");
    }
    /* print space to handle line less than 8 bytes */
    if (len < 8)
        fprintf(FILE_PTR, " ");
    
    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            fprintf(FILE_PTR, "   ");
        }
    }
    fprintf(FILE_PTR, "   ");
    
    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch))
            fprintf(FILE_PTR, "%c", *ch);
        else
            fprintf(FILE_PTR, ".");
        ch++;
    }

    fprintf(FILE_PTR, "\n");

    return;
}

/*
 * print packet payload data (avoid printing binary data)
 */
void print_payload(const u_char *payload, int len) {

    int len_rem = len;
    int line_width = 16;            /* number of bytes per line */
    int line_len;
    int offset = 0;                 /* zero-based offset counter */
    const u_char *ch = payload;

    if (len <= 0)
        return;

    /* data fits on one line */
    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        print_hex_ascii_line(ch, line_len, offset);
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }

    return;
}

/*
 * dissect/print packet
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    static int count = 1;                   /* packet counter */

    check_file();
    
    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */
    const u_char *payload;                    /* Packet payload */

    int size_ip;
    int size_tcp;
    int size_payload;
    
    fprintf(FILE_PTR, "\nPacket number %d:\n", count);
    printf("Packet number %d:\n", count);
    count++;
    
    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);
    
    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        fprintf(FILE_PTR, "   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    /* print source and destination IP addresses */
    fprintf(FILE_PTR, "       From: %s\n", inet_ntoa(ip->ip_src));
    fprintf(FILE_PTR, "         To: %s\n", inet_ntoa(ip->ip_dst));
    
    /* Time */
    char *current_time = get_current_time();
    fprintf(FILE_PTR, "   Time: %s\n", current_time);
    free(current_time);

    /* determine protocol */    
    switch(ip->ip_p) {
        case IPPROTO_TCP:
            fprintf(FILE_PTR, "   Protocol: TCP\n");
            break;
        case IPPROTO_UDP:
            fprintf(FILE_PTR, "   Protocol: UDP\n");
            return;
        case IPPROTO_ICMP:
            fprintf(FILE_PTR, "   Protocol: ICMP\n");
            return;
        case IPPROTO_IP:
            fprintf(FILE_PTR, "   Protocol: IP\n");
            return;
        default:
            fprintf(FILE_PTR, "   Protocol: unknown\n");
            return;
    }
    
    /*
     *  OK, this packet is TCP.
     */
    
    /* define/compute tcp header offset */
    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        fprintf(FILE_PTR, "   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }
    
    fprintf(FILE_PTR, "   Src port: %d\n", ntohs(tcp->th_sport));
    fprintf(FILE_PTR, "   Dst port: %d\n", ntohs(tcp->th_dport));
    
    /* define/compute tcp payload (segment) offset */
    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
    
    /* compute tcp payload (segment) size */
    size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
    
    /*
     * Print payload data; it might be binary, so don't just
     * treat it as a string.
     */
    if (size_payload > 0) {
        fprintf(FILE_PTR, "   Payload (%d bytes):\n", size_payload);
        print_payload(payload, size_payload);
    }

    return;
}

void initSniffer(char *dev){ 
    char errbuf[PCAP_ERRBUF_SIZE];      /* error buffer */
    pcap_t *handle;             /* packet capture handle */

    char filter_exp[] = "ip";       /* filter expression [3] */
    struct bpf_program fp;          /* compiled filter program (expression) */
    bpf_u_int32 mask;           /* subnet mask */
    bpf_u_int32 net;            /* ip */
    int num_packets = 0;           /* number of packets to capture */
    
    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
            dev, errbuf);
        net = 0;
        mask = 0;
    }

    /* print capture info */
    printf("Device: %s\n", dev);
    printf("Number of packets: %d\n", num_packets);
    printf("Filter expression: %s\n", filter_exp);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        exit(EXIT_FAILURE);
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* Open file */
    char *time_filename = get_current_time();
    snprintf(FILENAME, sizeof(FILENAME), "%s.txt", time_filename);
    free(time_filename);
    FILE_PTR = fopen(FILENAME, "wb");
    if (FILE_PTR == NULL) {
        fprintf(stderr, "Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    /* now we can set our callback function */
    pcap_loop(handle, num_packets, got_packet, NULL);

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);
    fclose(FILE_PTR);
    
    printf("\nCapture complete.\n");
}
