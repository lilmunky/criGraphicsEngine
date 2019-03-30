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
    static string folder;
    static string completePath;
    static const string fileName;
    static const string logFolder;
    static const string ext;
    static LoggerDestroyer destroyer;

    static void InitializeLogger();

    static void LogNoFormat(const string& text);

public:
    virtual ~Logger() = 0;

    static void InitLog();
    static void Log(const string& text);
    static void LogWindowsErrorCode(const string& context);
    static void LogD3DErrorCode(const string& context, const HRESULT& errorCode);
};

string CombinePaths(const string& p1, const string& p2);