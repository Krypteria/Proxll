#include <win32.h>

std::vector<char*> GetExportedFunctions(char* dllPath){
    HMODULE pDllAddr = LoadLibraryA(dllPath);

    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNtHeader;
    PIMAGE_OPTIONAL_HEADER pOpHeader;

    pDosHeader = (PIMAGE_DOS_HEADER)pDllAddr;

    if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE){
        printf("%s%s\n", ERR, "DOS Header signature mismatch\n");
        exit(EXIT_FAILURE);
    }
    
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)pDllAddr + pDosHeader->e_lfanew);
    if(pNtHeader->Signature != IMAGE_NT_SIGNATURE){
        printf("%s%s\n", ERR, "NT Header signature mismatch\n");
        exit(EXIT_FAILURE);
    }

    pOpHeader = &pNtHeader->OptionalHeader;

    IMAGE_DATA_DIRECTORY pExportedDir = pOpHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    PIMAGE_EXPORT_DIRECTORY pExportTable = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)pDllAddr + pExportedDir.VirtualAddress);

    PDWORD pFuncNames = (PDWORD)((DWORD_PTR)pDllAddr + pExportTable->AddressOfNames);

    std::vector<char*> exportedFunctions;
    for(size_t i = 0; i < pExportTable->NumberOfNames; i++){
        char *funcName = (char*)((DWORD_PTR)pDllAddr + pFuncNames[i]);
        exportedFunctions.push_back(funcName);
    }

    return exportedFunctions;
}