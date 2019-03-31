/* TODO: Get rid of this logger and use something thread-safe and simpler */

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
wstring Logger::folder;
wstring Logger::completePath;
const wstring Logger::fileName = L"logs";
const wstring Logger::logFolder = L"log";
const wstring Logger::ext = L".CElog";
LoggerDestroyer Logger::destroyer;

void Logger::InitializeLogger() {
    wchar_t appPath[MAX_PATH];
    GetModuleFileName(NULL, appPath, MAX_PATH);
    wstring::size_type pos = wstring(appPath).find_last_of(L'\\');
    wcscpy_s(appPath, wstring(appPath).substr(0, pos).c_str());

    folder = CombinePaths(appPath, logFolder);
    struct _stat64i32 info;
    if (_wstat(folder.c_str(), &info) != 0){
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

    wofstream logFile;
    time_t secEpoch = system_clock::to_time_t(system_clock::now());
    tm nowUCT;
    gmtime_s(&nowUCT, &secEpoch);
    logFile.open(completePath, ios_base::app);
    logFile << "Starting, " << (nowUCT.tm_year + 1900) << '-' << (nowUCT.tm_mon + 1) << '-' << nowUCT.tm_mday << "; " << 
        nowUCT.tm_hour << ':' << nowUCT.tm_min << ':' << nowUCT.tm_sec << endl;
    logFile.close();

    initialized = true;
}

void Logger::LogNoFormat(const wstring& text)
{
    if (!initialized) {
        InitializeLogger();
    }

    if (errorInit) {
        return;
    }

    wofstream logFile;
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

void Logger::Log(const wstring& text)
{
    if (!initialized) {
        InitializeLogger();
    }

    if (errorInit) {
        return;
    }

    wofstream logFile;
    time_t secEpoch = system_clock::to_time_t(system_clock::now());
    tm nowUCT;
    gmtime_s(&nowUCT, &secEpoch);
    logFile.open(completePath, ios_base::app);

    logFile << L'[' << nowUCT.tm_hour << L':' << nowUCT.tm_min << L':' << nowUCT.tm_sec << L'.' << 
        (duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 1000) << L"] ";
    logFile << text.c_str() << endl;
    
    logFile.close();
}

void Logger::LogWindowsErrorCode(const wstring& context)
{
    DWORD dwError = GetLastError();
    _com_error err(dwError);
    wstringstream strErr;
    strErr << hex << dwError;
    Logger::Log(L"Error " + context + L": 0x" + strErr.str() + L" - " + err.ErrorMessage());
}

void Logger::LogD3DErrorCode(const wstring& context, const HRESULT& errorCode)
{
    _com_error err(errorCode);
    wstringstream strErr;
    strErr << hex << errorCode;
    Logger::Log(L"Directx error - " + context + L": 0x" + strErr.str() + L" - " + err.ErrorMessage());
}

wstring CombinePaths(const wstring& p1, const wstring& p2) {

    if (p1.at(p1.length() - 1) == L'\\') {
        return p1 + p2;
    }

    return p1 + L'\\' + p2;
}

LoggerDestroyer::LoggerDestroyer()
{
}

LoggerDestroyer::~LoggerDestroyer()
{
    Logger::LogNoFormat(L"\n\n");
}
