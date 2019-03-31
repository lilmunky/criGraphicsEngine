#pragma once

#include <string>

class GraphicsD3D;

class WindowsUI {
    friend GraphicsD3D;

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
    bool IsFullscreen();

private:
    static int nWindows;
    HWND wndHandle;
    static HINSTANCE appInstance;
    const LPCSTR wndClassName = "Engine";
    LPCSTR wndName;
    bool isFullscreen;
    int insidePixelWidth;
    int insidePixelHeight;
    int totalPixelWidth;
    int totalPixelHeight;
    int defaultPixelWidth = 640;
    int defaultPixelHeight = 480;
    GraphicsD3D *graphics;


    bool Frame();
    bool InitializeWindow(const bool& setToFullScreen);
    bool RegisterWndClass();
    void UnregisterWndClass();

    LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MsgForwarderSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MsgForwarder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
