all:
	clang++ -dynamiclib -stdlib=libc++ Backup.cpp -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -o libBackup.A.dylib -v
	clang++ -std=c++11 Sniffer.cpp filetracker.cpp main.cpp libBackup.A.dylib -o main -I/usr/local/include -L/usr/local/lib -ltins -v
