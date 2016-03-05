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
};
