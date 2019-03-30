#pragma once

#include <string>

class WindowsUI {
public:
    WindowsUI();
    WindowsUI(const WindowsUI&) = delete;
    WindowsUI& operator=(const WindowsUI&) = delete;
    ~WindowsUI();

    bool NewWindow(const LPCSTR& windowName, const bool& setToFullScreen);
    void Shutdown();
    int Run();

    bool SetFullscreen(const bool& setToFullScreen);
    bool WindowExists();

private:
    static int nWindows;
    HWND wndHandle;
    static HINSTANCE appInstance;
    const LPCSTR wndClassName = "Engine";
    LPCSTR wndName;

    bool isFullscreen;
    int pixelWidth;
    int pixelHeight;
    int defaultPixelWidth = 640;
    int defaultPixelHeight = 480;

    bool Frame();
    bool InitializeWindow(const bool& setToFullScreen);
    bool RegisterWndClass();
    void UnregisterWndClass();
    void ShutdownWindow();

    LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MsgForwarderSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MsgForwarder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
