#pragma once

#include <iostream>

using namespace std;

class LoggerDestroyer {
public:
    LoggerDestroyer();
    ~LoggerDestroyer();
};

class Logger {
    friend LoggerDestroyer;

private:
    static bool initialized;
    static bool errorInit;
    static wstring folder;
    static wstring completePath;
    static const wstring fileName;
    static const wstring logFolder;
    static const wstring ext;
    static LoggerDestroyer destroyer;

    static void InitializeLogger();

    static void LogNoFormat(const wstring& text);

public:
    virtual ~Logger() = 0;

    static void InitLog();
    static void Log(const wstring& text);
    static void LogWindowsErrorCode(const wstring& context);
    static void LogD3DErrorCode(const wstring& context, const HRESULT& errorCode);
};

wstring CombinePaths(const wstring& p1, const wstring& p2);