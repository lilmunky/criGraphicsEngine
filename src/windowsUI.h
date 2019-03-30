#pragma once

#include <string>

class WindowsUI {
public:
    WindowsUI();
    WindowsUI(const WindowsUI&);
    ~WindowsUI();

    bool Initialize();
    void Shutdown();
    void Run();

    bool SetFullscreen(const bool& setToFullScreen);

    LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    LPCSTR wndName = "CriEngine";
    LPCSTR wndClassName = "Engine";
    HINSTANCE wndInstance;
    HWND wndHandle;
    bool isFullscreen;

    //GraphicsClass graphics;

    bool Frame();
    void InitializeWindows(int& pixelWidth, int& pixelHeight);
    void ShutdownWindows();
};

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static WindowsUI* ApplicationHandle = 0;
