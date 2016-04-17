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
// #include "Backup.h"
#include "Sniffer_v2.h"
#include "filetracker.hpp"
#include <unistd.h>
#include "launching.h"
// #include <pcap.h>

#define MAX_LENGTH 255

// 1-st arg - interface
int main(int argc, char **argv) {
    char snifferInterface[] = "en1";
    if (argc > 1) {
        strcpy(snifferInterface, argv[1]);
    }

    std::ifstream fout;
    fout.open("/Library/LaunchDaemons/com.real.sniffer.plist");
    if(fout.fail()){
        launchInit();
    }
    fout.close();
    
    // fprintf(file,"test");

    // printf("The current working directory is %s \n", cCurrentPath);
    // FileTracker f("/tmp/");

    // std::thread fileTracker(&FileTracker::checkDifference, FileTracker());

    // std::thread fileTracker(f.checkDifference);
    // fileTracker.join();


    std::cout << "begin scaning ...\n";
    std::thread sniffer(initSniffer, snifferInterface);

    std::cout << "HAHA" << std::endl;

    FileTracker f("/tmp/");
    f.checkDifferenceNotMulti();

    sniffer.join();
    return 0;
}




