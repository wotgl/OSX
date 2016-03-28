/* File: Backup.h
 * Interface to libCBackup.A.dylib 1.0.
 *************************************/
 
int init(char *, int);
int copyFile(const char *);
void GC(int num, ...);
int readConsole(char *input, size_t length);
void JeasusCopyFunction(int *err, int *copyFlag, size_t MAX_LENGTH);