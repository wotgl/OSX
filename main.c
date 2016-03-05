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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Backup.h"

#define MAX_LENGTH 255

int readConsole(char *, size_t);
void GC(int num, ...);


int main(int argc, char **argv) {
    char *containerPath, *fname;
    int err = 0;

    containerPath = (char*)malloc(MAX_LENGTH * sizeof(char));
    fname = (char*)malloc(MAX_LENGTH * sizeof(char));

    printf("Enter path to container:\n");
    err = readConsole(containerPath, MAX_LENGTH);
    if (err) {
        GC(2, containerPath, fname);
        printf("[error]:\tread path of the container fails;\n");
        return -1;
    }
    
    if (init(containerPath, MAX_LENGTH)) {
        GC(2, containerPath, fname);
        printf("[error]:\tinit of library failed;\n");
        return -1;
    }

    printf("Enter absolute path to file:\n");
    err = readConsole(fname, MAX_LENGTH);
    if (err) {
        GC(2, containerPath, fname);
        printf("[error]:\tread path of the file fails;\n");
        return -1;
    }

    if (copyFile(fname)) {
        GC(2, containerPath, fname);
        printf("[error]:\tcopy file fails;\n");
        return -1;
    }

	GC(2, containerPath, fname);
    printf("[done]\n");

    return 0;
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



