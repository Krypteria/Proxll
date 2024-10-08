#include <snippets.h>

const char* DEBUG = R"(
#include <stdio.h> 
#include <debugapi.h>

#define INFO "[*] - "
#define ERR "[!] - "
#define OK "[+] - "

#define _DEBUG
#ifdef _DEBUG
#define DEBUG_PRINT(fmt, str) \
        do { \
            char debug_buffer[1024]; \
            snprintf(debug_buffer, sizeof(debug_buffer), fmt "%s", str); \
            OutputDebugStringA(debug_buffer); \
        } while (0)
#else
#define DEBUG_PRINT(fmt, str) do { } while (0)
#endif
)";

const char* MAKEFILE = R"(MAKEFLAGS += -s

FLAGS = -s -masm=intel -static -shared
INCLUDE = -I ./include 
SOURCE = $(wildcard ./src/*.cpp) $(wildcard ./src/*.def)

EXECUTABLE = template.dll
	
x64:
	echo [*] Compiling proxy DLL (x64)
	
	x86_64-w64-mingw32-g++ -w  $(SOURCE) $(INCLUDE) $(FLAGS) -o ./bin/$(EXECUTABLE)
		
	echo [+] Proxy DLL generated at \template\bin

)";

const char* FIND_EXPORTED_FUNC = R"(
    HMODULE hDll = LoadLibraryA(dllPath);
    if(hDll == NULL){
        DEBUG_PRINT(ERR, "Error loading DLL\n");
        exit(EXIT_FAILURE);
    }

    FARPROC pFunction = GetProcAddress(hDll, targetedFunc);
    if(pFunction == NULL){
        DEBUG_PRINT(ERR, "Error searching function\n");
        exit(EXIT_FAILURE);
    }

    DEBUG_PRINT(OK, "Address of the real function obtained\n");
)";

const char* EXECUTE_PAYLOAD = R"(
    std::thread tPayload(executePayload);
    if (tPayload.joinable()) {
        tPayload.join();
    } 
    else {
        DEBUG_PRINT(ERR, "Error during execution\n");
        exit(EXIT_FAILURE);
    }
)";

const char* EXECUTE_PAYLOAD_MAIN = R"(
    executePayload();
)";

const char* DEFINE_TRAMPOLINE = R"(
    proxllTrampoline PFuncTrampoline = (proxllTrampoline)pFunction; 
)";

const char* PAYLOAD = R"(
void executePayload(){
    if(!executed){
        DEBUG_PRINT(INFO, "Executing Payload\n");
        executed = true;

        // YOUR CODE HERE

        DEBUG_PRINT(OK, "Payload executed\n");
    }
}
)";

const char* DLLMAIN_CONTENT = R"(
    switch(fdwReason){ 
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE; 
}
)";

const char* DLLMAIN_CONTENT_TARGETED = R"(
    switch(fdwReason){ 
        case DLL_PROCESS_ATTACH:
            DEBUG_PRINT(INFO, "DllMain called\n");
            proxllCore();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE; 
}
)";