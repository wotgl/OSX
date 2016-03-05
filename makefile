all:
	clang -dynamiclib -std=gnu99 Backup.c -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -o libBackup.A.dylib
	clang main.c libBackup.A.dylib -o main
