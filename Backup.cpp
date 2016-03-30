/* File: Backup.c
 * Compile with -fvisibility=hidden.
 **********************************/

/*
	*************************************************
	
	ATTENTION!
	Everywhere below, the style is applied to golang:
		function returns 0 - OK;
		function returns !0 - NOT OK;

	*************************************************
*/
 
#include <iostream>
#include "Backup.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


 
#define EXPORT __attribute__((visibility("default")))

char *PATH;
char NEW_NAME[] = "/file.backup";
size_t MAX_LENGTH;
 
// Initializer.
__attribute__((constructor))
static void initializer(void) {
    printf("[%s] initializer()\n", __FILE__);
}
 
// Finalizer.
__attribute__((destructor))
static void finalizer(void) {
    printf("[%s] finalizer()\n", __FILE__);
}

/*
    Check validity of the path;
*/
int checkPath(const char* fname) {
    int ok = 1;
    FILE *f;

    if ((f = fopen(fname, "r")) != NULL) {
    	fclose(f);
		ok = 0;
	}
    return ok;
}
 
/*
	Init container;
*/
EXPORT
int init(char *CONTAINER_PATH, int _MAX_LENGTH) {
	PATH = CONTAINER_PATH;
	MAX_LENGTH = _MAX_LENGTH;

	if (checkPath(PATH)) {
		return 1;
	}

	return 0;
}


/*
    Like Garbage Collection;
    input: number of pointers, pointers;
*/
EXPORT
void GC(int num, ...) {
    va_list arguments;
    va_start(arguments, num);

    for (int x = 0; x < num; x++) {
        free(va_arg(arguments, void*));
    }
    va_end(arguments);
}

/*
    Read data from console;
    input: pointer to char array, max length;
*/
EXPORT
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
    Create copy of the file;
    input: path of the file;
*/
EXPORT
int copyFile(const char *fname) {
	FILE *src, *dest;
	char *fullPath, tmp;

	src = fopen(fname, "r");
	if (src == NULL)
		return 1;

	fullPath = (char*)malloc(sizeof(char) * 2 * MAX_LENGTH);
	strcpy(fullPath, PATH);
	strcat(fullPath, NEW_NAME);

	dest = fopen(fullPath, "w");
	if (dest == NULL) {
		fclose(src);
		free(fullPath);
		return 1;
	}

	do {
		tmp = fgetc(src);
		fputc(tmp, dest);
   	} while (tmp != EOF);

	fclose(src);
	fclose(dest);
	free(fullPath);

	return 0;
}

// EXPORT
// void JeasusCopyFunction(int *err, int *copyFlag, size_t MAX_LENGTH){
//     char *containerPath = (char*)malloc(MAX_LENGTH * sizeof(char));
//     char *fname = (char*)malloc(MAX_LENGTH * sizeof(char));
//     printf("Enter path to container:\n");
//     *err = readConsole(containerPath, MAX_LENGTH);
//     if (*err) {
//         GC(2, containerPath, fname);
//         printf("[error]:\tread path of the container fails;\n");
//         *copyFlag = -1;
//         return;
//     }
    
//     if (init(containerPath, MAX_LENGTH)) {
//         GC(2, containerPath, fname);
//         printf("[error]:\tinit of library failed;\n");
//         *copyFlag = -1;
//         return;
//     }
    
//     printf("Enter absolute path to file:\n");
//     *err = readConsole(fname, MAX_LENGTH);
//     if (*err) {
//         GC(2, containerPath, fname);
//         printf("[error]:\tread path of the file fails;\n");
//         *copyFlag = -1;
//         return;
//     }
    
//     if (copyFile(fname)) {
//         GC(2, containerPath, fname);
//         printf("[error]:\tcopy file fails;\n");
//         *copyFlag = -1;
//         return;
//     }
    
//     GC(2, containerPath, fname);
//     printf("[done]\n");
//     *copyFlag = 0;
//     return;
// };


