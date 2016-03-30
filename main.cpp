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
#include "Backup.h"
#include "Sniffer.h"
#include "filetracker.hpp"

#define MAX_LENGTH 255


// 1-st arg - interface
int main(int argc, char **argv){
    std::string snifferInterface = "en0";   
    if (argc > 1) {
        snifferInterface = argv[1];
    }

    // FileTracker f("/tmp/");

    // std::thread fileTracker(&FileTracker::checkDifference, FileTracker());

    // std::thread fileTracker(f.checkDifference);
    // fileTracker.join();


    

    std::cout << "begin scaning ...\n";
    std::thread sniffer(initSniffer, snifferInterface);

    std::cout << "HAHA" << std::endl;

    FileTracker f("/tmp/");
    f.checkDifference();
    sniffer.join();
    return 0;
}





