#include <generator.h>

// Utilities 
// --- 
std::string replaceBackslashes(const char* input) {
    std::string str(input);
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\\') {
            str.insert(i, "\\");
            i++; 
        }
    }
    return str;
}

char* toLower(char* input) {
    size_t inputLen = strlen(input) + 1;
    char* output = new char[inputLen];

    for (size_t i = 0; input[i]; i++) {
        output[i] = std::tolower(input[i]);
    }

    output[inputLen - 1] = '\0';

    return output;
}

void createFolder(const char* folderPath) {
    if (!std::filesystem::create_directory(folderPath)) {
        printf("%s%s\n", ERR, "Error, folder already exists");
    } 
}

void deleteFolderContents(const std::string& folderPath) {
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_directory()) {
            deleteFolderContents(entry.path().string());
            std::filesystem::remove(entry.path());
        } else {
            if (entry.path().filename() != ".gitkeep") {
                std::filesystem::remove(entry.path());
            }
        }
    }
}

void saveToFile(const char* generatedTemplate, const char* basePath, const char* fileName){
    size_t fullPathSize = strlen(basePath) + strlen(fileName) + 1;
    char* fullPath = new char[fullPathSize];

    strcpy(fullPath, basePath);
    strcat(fullPath, fileName);

    std::ofstream outFile(fullPath, std::ios_base::app);
    if(!outFile){
        printf("%s%s\n", ERR, "Error while opening output file\n");
        exit(EXIT_FAILURE);
    }

    outFile << generatedTemplate;
    outFile.close();
}

// ---

// Gen functions
// ---

void generateTemplateFolders(){
    char* BASE_DIR = "..\\..\\template";
    if(std::filesystem::exists(BASE_DIR) && std::filesystem::is_directory(BASE_DIR)){
        deleteFolderContents(BASE_DIR);
    }

    createFolder(INCLUDE_PATH);
    createFolder(SRC_PATH);
    createFolder(BIN_PATH);
}

std::tuple<char*,char*> generateArguments(){
    std::stringstream typedFuncArgs;
    std::stringstream rawFuncArgs;

    for(size_t i = 0; i < 15; i++){    
        typedFuncArgs << "void* arg" << i;
            rawFuncArgs << "arg" << i;

        if(i != 14){
            typedFuncArgs << ",";
            rawFuncArgs << ",";
        }
    }

    char* typedFuncArguments = new char[(typedFuncArgs.str()).size() + 1];
    char* rawFuncArguments = new char[(rawFuncArgs.str()).size() + 1];

    strcpy(typedFuncArguments, (typedFuncArgs.str()).c_str());
    strcpy(rawFuncArguments, (rawFuncArgs.str()).c_str());
    
    return std::make_tuple(typedFuncArguments, rawFuncArguments);
}

void generateTCore(char* dllPath, char* targetedFunc){
    std::stringstream coreTemplate;
    char* typedFuncArguments; 
    char* rawFuncArguments;

    std::tie(typedFuncArguments, rawFuncArguments) = generateArguments();

    coreTemplate << "#include <windows.h>\n#include <thread>\n\n#include <t_core.h>\n\nstatic bool executed = false;\n\n";

    if(strcmp(toLower(targetedFunc), "dllmain") == 0){
        coreTemplate << PAYLOAD << "\n";
        coreTemplate << "void proxllCore(){";
        coreTemplate << EXECUTE_PAYLOAD_MAIN << "}\n\n";
    }
    else{ //Exported function
        coreTemplate << "typedef void* (*proxllTrampoline)(" << typedFuncArguments << ");\n\n";
        coreTemplate << "char* dllPath = \"" << replaceBackslashes(dllPath) << "\";\n";
        coreTemplate << "char* targetedFunc = \"" << targetedFunc << "\";\n\n";

        coreTemplate << PAYLOAD << "\n";
        coreTemplate << "void* proxllCore(" << typedFuncArguments << "){";
        coreTemplate << FIND_EXPORTED_FUNC << EXECUTE_PAYLOAD << DEFINE_TRAMPOLINE;
        coreTemplate << "\treturn PFuncTrampoline(" << rawFuncArguments << ");\n}\n\n";
    }

    coreTemplate << "BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){";
    
    if(strcmp(toLower(targetedFunc), "dllmain") == 0){
        coreTemplate << DLLMAIN_CONTENT_TARGETED;
    }
    else{
        coreTemplate << DLLMAIN_CONTENT;
    }

    saveToFile((coreTemplate.str()).c_str(), SRC_PATH, "t_core.cpp");
}

void generateTCore_H(char* targetedFunc){
    std::stringstream coreHTemplate;

    char* typedFuncArguments = "";
    char* rawFuncArguments = "";

    coreHTemplate << "#ifndef TCORE_H\n#define TCORE_H\n\n";
    coreHTemplate << DEBUG << "\n\n";

    if(strcmp(toLower(targetedFunc), "dllmain") != 0){
        
        std::tie(typedFuncArguments, rawFuncArguments) = generateArguments();

        coreHTemplate << "#include <cstdint>\n\nvoid* proxllCore(" << typedFuncArguments << ");";
    }

    coreHTemplate << "\n#endif";

    saveToFile((coreHTemplate.str()).c_str(), INCLUDE_PATH, "t_core.h");
}

void generateTExported(char* dllPath, char* targetedFunc){
    std::vector<char*> exportedFunctions = GetExportedFunctions(dllPath);

    std::stringstream exportedTemplate;
    char* typedFuncArguments = "";
    char* rawFuncArguments = "";

    char* funcCode = "";

    if(strcmp(toLower(targetedFunc), "dllmain") != 0){
        exportedTemplate << "#include <t_core.h>\n\n";
    }

    exportedTemplate << "#define EXPORT __declspec(dllexport)\n\n";

    for(size_t i = 0; i < exportedFunctions.size(); i++){
        if(strcmp(toLower(exportedFunctions[i]), toLower(targetedFunc)) == 0){
            std::tie(typedFuncArguments, rawFuncArguments) = generateArguments();
            std::stringstream targetedCall;

            targetedCall << "{\n\tDEBUG_PRINT(INFO, \"Exported function called\\n\");\n";
            targetedCall << "\tproxllCore(" << rawFuncArguments << ");\n}";

            funcCode = new char[(targetedCall.str()).size() + 1];
            strcpy(funcCode, (targetedCall.str()).c_str());
        }
        else{
            typedFuncArguments = "";
            funcCode = "{}";
        }

        exportedTemplate << "extern \"C\" EXPORT void " << exportedFunctions[i] << "(" << typedFuncArguments << ")" << funcCode << "\n\n";
    }   

    saveToFile((exportedTemplate.str()).c_str(), SRC_PATH, "t_exported.cpp");
}

void generateDEF(char* dllPath, char* targetedFunc){
    std::vector<char*> exportedFunctions = GetExportedFunctions(dllPath);

    size_t lenghtWithoutExt = strlen(dllPath) - 4;
    char* dllExportPath = new char[lenghtWithoutExt + 1];
    strncpy(dllExportPath, dllPath, lenghtWithoutExt);
    dllExportPath[lenghtWithoutExt] = '\0';

    std::stringstream defTemplate;
    defTemplate << "EXPORTS\n";

    bool funcFound = false;
    for(size_t i = 0; i < exportedFunctions.size(); i++){
        if(strcmp(toLower(exportedFunctions[i]), toLower(targetedFunc)) != 0){
            defTemplate << exportedFunctions[i] << "=\"" << dllExportPath << "." << exportedFunctions[i] << "\"\n";
        }
        else{
            funcFound = true;
        }
    }

    if(!funcFound && (strcmp(toLower(targetedFunc), toLower("dllmain")) != 0)){
        printf("%s%s\n", ERR, "The chosen function doesn't exist");
        exit(EXIT_FAILURE);
    }

    saveToFile((defTemplate.str()).c_str(), SRC_PATH, "exported.def");
}

void generateMAKE(){
    std::stringstream makeTemplate;

    makeTemplate << MAKEFILE;
    saveToFile((makeTemplate.str()).c_str(), BASE_PATH, "makefile"); 
}

bool isDLL(char* dllPath){
    if (!std::filesystem::exists(dllPath)) {
        printf("%s%s\n", ERR, "The chosen DLL doesn't exist");
        return false;
    }

    size_t dllPathLen = strlen(dllPath);
    if (strcmp(dllPath + dllPathLen - 4, ".dll") == 0) {
        return true;
    } else {
        printf("%s%s\n", ERR, "The chosen DLL is not valid");
        return false;
    }
}

void generateProxyTemplate(char* dllPath, char* targetedFunc){
    if(!isDLL(dllPath)){
        exit(EXIT_FAILURE);
    }
    
    generateTemplateFolders();

    generateTCore(dllPath, targetedFunc);
    generateTExported(dllPath, targetedFunc);
    generateTCore_H(targetedFunc);
    generateDEF(dllPath, targetedFunc);
    generateMAKE();

    printf("%s%s\n", OK, "Template generated at \\Proxll\\template");
}

// --- 