#pragma once

#include <string>

class GraphicsController;

class WindowsUI {
    friend GraphicsController;

public:
    WindowsUI();
    WindowsUI(const WindowsUI&) = delete;
    WindowsUI& operator=(const WindowsUI&) = delete;
    ~WindowsUI();

    bool NewWindow(const LPCWSTR& windowName, const bool& setToFullScreen);
    void Shutdown();
    int Run();

    bool SetFullscreen(const bool& setToFullScreen);
    bool WindowExists();

private:
    static int nWindows;
    HWND wndHandle;
    static HINSTANCE appInstance;
    const LPCWSTR wndClassName = L"Engine";
    LPCWSTR wndName;
    bool isFullscreen;
    int insidePixelWidth;
    int insidePixelHeight;
    int totalPixelWidth;
    int totalPixelHeight;
    int defaultPixelWidth = 640;
    int defaultPixelHeight = 480;
    GraphicsController *childGraphics;


    bool Frame();
    bool InitializeWindow(const bool& setToFullScreen);
    bool RegisterWndClass();
    void UnregisterWndClass();

    LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MsgForwarderSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MsgForwarder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
