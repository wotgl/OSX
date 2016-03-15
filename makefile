all:
	clang++ -dynamiclib -stdlib=libc++ Backup.cpp -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -o libBackup.A.dylib
	clang++ -std=c++11 main.cpp libBackup.A.dylib -o main -I/usr/local/include -L/usr/local/lib -ltins
