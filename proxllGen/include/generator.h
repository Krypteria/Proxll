#ifndef GENERATOR_H
#define GENERATOR_H

#include <sstream>
#include <fstream>
#include <tuple>
#include <string>
#include <cstring>
#include <cctype>
#include <filesystem>

#include <win32.h>
#include <snippets.h>

#define BASE_PATH "..\\..\\template\\"
#define INCLUDE_PATH "..\\..\\template\\include\\"
#define SRC_PATH "..\\..\\template\\src\\"
#define BIN_PATH "..\\..\\template\\bin\\"

void saveToFile(const char* generatedTemplate, const char* basePath, const char* fileName);

void generateTCore(char* dllPath, char* targetedFunc);
void generateTCore_H(char* targetedFunc);
void generateTExported(char* dllPath, char* targetedFunc);
void generateDEF(char* dllPath, char* targetedFunc);
void generateMAKE();
void generateProxyTemplate(char* dllPath, char* targetedFunc);

std::tuple<char*,char*> generateArguments();

void generateTemplateFolders();
void createFolder(const char* folderPath);
void deleteFolderContents(const std::string& folderPath);

std::string replaceBackslashes(const char* input);
char* toLower(char* input);
bool isDLL(char* dllPath);

#endif