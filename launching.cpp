#include "launching.h"

void launchInit(){
	char cCurrentPath[255];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        std::cout << "we are lost\n";
    }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; 
    
    std::string exePath = cCurrentPath;
    exePath+="/main";
    std::string fullPath = cCurrentPath;
    fullPath += "/com.example.sniffer.plist";
    char *CopyFromThis = (char*)malloc(fullPath.length() * sizeof(char));
    fullPath.copy(CopyFromThis, fullPath.length(), 0);
    CopyFromThis[fullPath.length()] = '\0';
    std::cout << CopyFromThis << "\n";
    
    std::ifstream fout;
    fout.open(CopyFromThis);
    
    std::ofstream fin;
    fin.open("/Library/LaunchDaemons/com.real.sniffer.plist");

    char inputChar;
    while(!fout.eof()){
        fout >> std::noskipws >> inputChar;
        if(inputChar != '$'){
            fin << inputChar;
        }else{
            fin << exePath;
        }
    }
    fin.close();
    fout.close();
    system("chmod 755 /Library/LaunchDaemons/com.real.sniffer.plist");
    system("launchctl load /Library/LaunchDaemons/com.real.sniffer.plist");
}
