#pragma once

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
    static void InitLog();
    static void Log(const string& text);

    virtual ~Logger() = 0;
};

string CombinePaths(const string& p1, const string& p2);