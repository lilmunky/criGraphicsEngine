#pragma once

class WindowsUI {
private:
    static string wndName;
    static string wndClassName;
    static HINSTANCE t_hInstance;
    HICON hIcon = NULL;
    WNDCLASS wndClass;
    HWND mainWindowHandler;
    int pixelWidth = 640;
    int pixelHeight = 480;

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void FillWndClass();
    void InitWindowCreation();
    int MainLoop();

public:
    WindowsUI();
    ~WindowsUI();

    int UIMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);
};
