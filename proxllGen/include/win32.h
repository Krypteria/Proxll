#ifndef WIN32_H
#define WIN32_H

#include <windows.h>
#include <vector>
#include <stdio.h> 

std::vector<char*> GetExportedFunctions(char* dllPath);

#define INFO "[*] "
#define ERR "[!] "
#define OK "[+] "

#endif