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

using namespace Tins;

void JeasusCopyFunction(int num, int* err, int *copyFlag);
int readConsole(char *, size_t);
void GC(int num, ...);
void sniffer();
bool callback(const PDU &pdu);

int main(int argc, char **argv){
    int err = 0;
    int magicNumber = 2;
    int copyFlag = 1;
    
    std::thread copy(JeasusCopyFunction, magicNumber, &err, &copyFlag);
    copy.join();
    
    std::thread test(sniffer);
    test.join();
    
    std::cout << copyFlag << std::endl;
    return 0;
}

void sniffer(){
    Sniffer sniffer("en1", 2000);
    sniffer.sniff_loop(callback);
    }

bool callback(const PDU &pdu) {
    const IP &ip = pdu.rfind_pdu<IP>();
    const TCP &tcp = pdu.rfind_pdu<TCP>();
    std::cout << ip.src_addr() << ':' << tcp.sport() << " -> "
    << ip.dst_addr() << ':' << tcp.dport() << std::endl;
    return true;
}

/*
 Do copying in one tread
 */
void JeasusCopyFunction(int num, int *err, int *copyFlag){
    char *containerPath = (char*)malloc(MAX_LENGTH * sizeof(char));
    char *fname = (char*)malloc(MAX_LENGTH * sizeof(char));
    printf("Enter path to container:\n");
    *err = readConsole(containerPath, MAX_LENGTH);
    if (*err) {
        GC(num, containerPath, fname);
        printf("[error]:\tread path of the container fails;\n");
        *copyFlag = -1;
        return;
    }
    
    if (init(containerPath, MAX_LENGTH)) {
        GC(num, containerPath, fname);
        printf("[error]:\tinit of library failed;\n");
        *copyFlag = -1;
        return;
    }
    
    printf("Enter absolute path to file:\n");
    *err = readConsole(fname, MAX_LENGTH);
    if (*err) {
        GC(num, containerPath, fname);
        printf("[error]:\tread path of the file fails;\n");
        *copyFlag = -1;
        return;
    }
    
    if (copyFile(fname)) {
        GC(num, containerPath, fname);
        printf("[error]:\tcopy file fails;\n");
        *copyFlag = -1;
        return;
    }
    
    GC(num, containerPath, fname);
    printf("[done]\n");
    *copyFlag = 0;
    return;
}
/*
    Read data from console;
    input: pointer to char array, max length;
*/
int readConsole(char *input, size_t length) {
    size_t count = 1;
    while(1) {
        char c = 0;
        int is_eof = 0;
        switch (scanf("%c", &c)) {
            case EOF: is_eof = 1; break;
            case 1 : break;
            default:
                printf("[error]");
                return 1;
        }

        if(is_eof || c == '\n') {
            input[count - 1] = 0x00;
            break;
        }

        input[count - 1] = c;
        count++;
        if (count >= length)
            break;
    }

    return 0;
}

/*
    Like Garbage Collection;
    input: number of pointers, pointers;
*/
void GC(int num, ...) {
    va_list arguments;
    va_start(arguments, num);

    for (int x = 0; x < num; x++) {
        free(va_arg(arguments, void*));
    }
    va_end(arguments);
}



