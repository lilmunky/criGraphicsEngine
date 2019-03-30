#include "thin_windows.h"
#include <iostream>
#include "logger.h"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>
#include <ctime>
#include <comdef.h>
#include <sstream>

using namespace chrono;

bool Logger::errorInit = false;
bool Logger::initialized = false;
string Logger::folder;
string Logger::completePath;
const string Logger::fileName = "logs";
const string Logger::logFolder = "log";
const string Logger::ext = ".CElog";
LoggerDestroyer Logger::destroyer;

void Logger::InitializeLogger() {
    char appPath[MAX_PATH];
    GetModuleFileName(NULL, appPath, MAX_PATH);
    string::size_type pos = string(appPath).find_last_of("\\");
    strcpy_s(appPath, string(appPath).substr(0, pos).c_str());

    folder = CombinePaths(appPath, logFolder);
    struct stat info;
    if (stat(folder.c_str(), &info) != 0){
        if (errno == ENOENT) {
            if (CreateDirectory(folder.c_str(), NULL) == 0) {
                errorInit = true;
                return;
            }
        }
        else {
            errorInit = true;
            return;
        }
    }
    else if (!(info.st_mode & S_IFDIR)) {
        errorInit = true;
        return;
    }

    completePath = CombinePaths(folder, fileName) + ext;

    ofstream logFile;
    time_t secEpoch = system_clock::to_time_t(system_clock::now());
    tm nowUCT;
    gmtime_s(&nowUCT, &secEpoch);
    logFile.open(completePath, ios_base::app);
    logFile << "Starting, " << (nowUCT.tm_year + 1900) << '-' << (nowUCT.tm_mon + 1) << '-' << nowUCT.tm_mday << "; " << 
        nowUCT.tm_hour << ':' << nowUCT.tm_min << ':' << nowUCT.tm_sec << endl;
    logFile.close();

    initialized = true;
}

void Logger::LogNoFormat(const string& text)
{
    if (!initialized) {
        InitializeLogger();
    }

    if (errorInit) {
        return;
    }

    ofstream logFile;
    logFile.open(completePath, ios_base::app);
    logFile << text.c_str();
    logFile.close();
}

void Logger::InitLog()
{
    if (!(initialized || errorInit)) {
        InitializeLogger();
    }
}

void Logger::Log(const string& text)
{
    if (!initialized) {
        InitializeLogger();
    }

    if (errorInit) {
        return;
    }

    ofstream logFile;
    time_t secEpoch = system_clock::to_time_t(system_clock::now());
    tm nowUCT;
    gmtime_s(&nowUCT, &secEpoch);
    logFile.open(completePath, ios_base::app);

    logFile << '[' << nowUCT.tm_hour << ':' << nowUCT.tm_min << ':' << nowUCT.tm_sec << '.' << 
        (duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 1000) << "] ";
    logFile << text.c_str() << endl;
    
    logFile.close();
}

void Logger::LogWindowsErrorCode(const string& context)
{
    DWORD dwError = GetLastError();
    _com_error err(dwError);
    stringstream strErr;
    strErr << hex << dwError;
    Logger::Log("Error " + context + ": 0x" + strErr.str() + " - " + err.ErrorMessage());
}

void Logger::LogD3DErrorCode(const string& context, const HRESULT& errorCode)
{
    _com_error err(errorCode);
    stringstream strErr;
    strErr << hex << errorCode;
    Logger::Log("Directx error - " + context + ": 0x" + strErr.str() + " - " + err.ErrorMessage());
}

string CombinePaths(const string& p1, const string& p2) {

    if (p1.at(p1.length() - 1) == '\\') {
        return p1 + p2;
    }

    return p1 + "\\" + p2;
}

LoggerDestroyer::LoggerDestroyer()
{
}

LoggerDestroyer::~LoggerDestroyer()
{
    Logger::LogNoFormat("\n\n");
}
