#include "thin_windows.h"
#include "windowsUI.h"
#include "logger.h"

WindowsUI::WindowsUI()
{
}

WindowsUI::WindowsUI(const WindowsUI &)
{
}

WindowsUI::~WindowsUI()
{
}

bool WindowsUI::Initialize()
{
    int pixelWidth, pixelHeight;
    bool result;

    pixelWidth = 0;
    pixelHeight = 0;

    InitializeWindows(pixelWidth, pixelHeight);

    return true;
}

void WindowsUI::Shutdown()
{
    ShutdownWindows();

    return;
}

void WindowsUI::Run()
{
    MSG msg;
    bool done, result;

    ZeroMemory(&msg, sizeof(MSG));

    done = false;
    while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }
}

bool WindowsUI::SetFullscreen(const bool & setToFullScreen)
{
    DWORD wndStyle = 0;
    LONG_PTR result;

    if (setToFullScreen && !isFullscreen) {
        wndStyle |= WS_POPUP;
        result = SetWindowLongPtr(wndHandle, GWL_STYLE, wndStyle);

        if (!result) {
            Logger::LogWindowsErrorCode("setting window to fullscreen");
            return false;
        }

        isFullscreen = true;
        return true;
    }

    if (!setToFullScreen) {
        wndStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
        result = SetWindowLongPtr(wndHandle, GWL_STYLE, wndStyle);

        if (!result) {
            Logger::LogWindowsErrorCode("setting window to windowed");
            return false;
        }
    }

    return true;
}

LRESULT WindowsUI::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        default:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
}

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        default:
        {
            return ApplicationHandle->MessageHandler(hWnd, uMsg, wParam, lParam);
        }
    }
}

bool WindowsUI::Frame()
{
    return true;
}

void WindowsUI::InitializeWindows(int & pixelWidth, int & pixelHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    DWORD wndStyle = 0;

    ApplicationHandle = this;

    wndInstance = GetModuleHandle(NULL);

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = wndInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = wndClassName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    pixelWidth = GetSystemMetrics(SM_CXSCREEN);
    pixelHeight = GetSystemMetrics(SM_CYSCREEN);

    if (false) //(FULL_SCREEN)
    {
        ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)pixelWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)pixelHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = posY = 0;

        wndStyle |= WS_POPUP;

        isFullscreen = true;
    }
    else
    {
        pixelWidth = 640;
        pixelHeight = 480;

        posX = (GetSystemMetrics(SM_CXSCREEN) - pixelWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - pixelHeight) / 2;

        wndStyle |= WS_SYSMENU | WS_MINIMIZEBOX;

        isFullscreen = false;
    }

    wndHandle = CreateWindowEx(
        WS_EX_APPWINDOW, 
        wndClassName, 
        wndName,
        wndStyle,
        posX,
        posY,
        pixelWidth,
        pixelHeight,
        NULL,
        NULL,
        wndInstance,
        NULL
    );

    ShowWindow(wndHandle, SW_SHOW);
    SetForegroundWindow(wndHandle);
    SetFocus(wndHandle);

    return;
}

void WindowsUI::ShutdownWindows()
{
    if (false) //(FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    DestroyWindow(wndHandle);
    wndHandle = NULL;

    UnregisterClass(wndClassName, wndInstance);
    wndInstance = NULL;

    ApplicationHandle = NULL;
}
