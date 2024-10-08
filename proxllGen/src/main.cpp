#include <generator.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("[!] - Usage: proxllGen.exe <DLL path> <target function>\n");
        exit(EXIT_FAILURE);
    }

    char* dllPath = argv[1];
    char* targetedFunc = argv[2];

    generateProxyTemplate(dllPath, targetedFunc);
    
    return true;
}