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

#include <stdarg.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <fstream>
#include "Backup.h"
#include "Sniffer_v2.h"
#include "filetracker.hpp"
#include <unistd.h>
#include "launching.h"

#define MAX_LENGTH 255

// 1-st arg - interface
int main(int argc, char **argv) {
    char snifferInterface[128] = "en1";
    if (argc > 1) {
        if(strlen(argv[1]) > 127){
            std::cout << "trying to overload\n";
            exit(-1);
        }
        strcpy(snifferInterface, argv[1]);
    }

    std::ifstream fout;
    fout.open("/Library/LaunchDaemons/com.real.sniffer.plist");
    if(fout.fail()){
        launchInit();
    }
    fout.close();

    std::cout << "begin scaning ...\n";
    std::thread sniffer(initSniffer, snifferInterface);

    

    FileTracker f("/tmp/");
    f.checkDifferenceNotMulti();
    std::cout << "HAHA" << std::endl;
    sniffer.join();
    return 0;
}




