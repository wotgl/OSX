/* main.c
 * Tests libRatings.A.dylib 1.0 as a dependent library.
 *****************************************************/

/*
    *************************************************
    
    ATTENTION!
    Everywhere below, the style is applied to golang:
        function returns 0 - OK;
        function returns !0 - NOT OK;

    *************************************************
*/

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <thread>
#include <tins/tins.h>
#include "Backup.h"

#define MAX_LENGTH 255


void sniffer();
bool callback(const Tins::PDU &pdu);

int main(int argc, char **argv){
    int err = 0;
    int copyFlag = 1;
    
    std::thread copy(JeasusCopyFunction, &err, &copyFlag, MAX_LENGTH);
    copy.join();
    std::cout << "errorr : " << copyFlag << std::endl;
    
    std::cout << "begin scaning ...\n";
    std::thread test(sniffer);
    test.join();
    
//    std::cout << "errorr : " << copyFlag << std::endl;
    return 0;
}

void sniffer(){
    Tins::Sniffer sniffer("en1", 2000);
    sniffer.sniff_loop(callback);
    }

bool callback(const Tins::PDU &pdu) {
    const Tins::IP &ip = pdu.rfind_pdu<Tins::IP>();
    const Tins::TCP &tcp = pdu.rfind_pdu<Tins::TCP>();
    std::cout << ip.src_addr() << ':' << tcp.sport() << " -> "
    << ip.dst_addr() << ':' << tcp.dport() << std::endl;
    return true;
}




