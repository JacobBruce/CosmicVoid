#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <sys/stat.h>
#include "Resource.h"
#include "StringExt.h"
#include "MathExt.h"

#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #define access   _access_s
    #define stat64   _stat64
    #define mkdir    _mkdir
#else
    #include <unistd.h>
#endif

bool DirExists(std::string dirname);

bool CreateDir(std::string dirname);

bool FileExists(const std::string& filename);

long long FileSize(std::string& filename);

void PrintLine(std::string str);

void PrintLine(std::stringstream& str_stream);

void HandleFatalError(const int ecode, std::string emsg);

std::string ReadFileStr(const std::string filename);

std::vector<std::string> ReadFileLines(const std::string filename);

bool LoadConfigFile(const std::string filename, std::unordered_map<std::string,std::string>& str_map);

bool LoadLangFile(const std::string filename, std::vector<std::pair<std::string,std::string>>& str_vec);

void CLBLog(const std::string logstr);
