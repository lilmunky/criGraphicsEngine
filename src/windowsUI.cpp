#include "thin_windows.h"
#include "logger.h"
#include "graphicscontroller.h"
#include "windowsUI.h"

int WindowsUI::nWindows = 0;

HINSTANCE WindowsUI::appInstance = NULL;

WindowsUI::WindowsUI()
{
    appInstance = 0;
    wndHandle = 0;

    childGraphics = NULL;
}

WindowsUI::~WindowsUI()
{
}

bool WindowsUI::NewWindow(const LPCWSTR & windowName, const bool & setToFullScreen)
{
    bool errors = false;
    if (wndHandle != 0) {
        Logger::Log(L"Tried to create a new window, but old window handle was found for this instance");
        return false;
    }

    wndName = windowName;
    childGraphics = new GraphicsController;
    if (!childGraphics) {
        Logger::Log(L"Could not create new graphics object.");
        return false;
    }

    errors = !InitializeWindow(setToFullScreen);
    errors |= !childGraphics->Initialize(reinterpret_cast<WindowsUI*>(this));

    return !errors;
}

void WindowsUI::Shutdown()
{
    if (childGraphics) {
        childGraphics->Shutdown();
        delete childGraphics;
        childGraphics = 0;
    }

    if (wndHandle) {
        DestroyWindow(wndHandle);
        wndHandle = NULL;
        nWindows--;
    }

    if (nWindows == 0) {
        UnregisterWndClass();
    }
}

int WindowsUI::Run()
{
    if (wndHandle == NULL) {
        Logger::Log(L"Tried to execute Run but no active window was found associated with this WindowsUI instance.");
        return 1;
    }

    MSG msg;

    ZeroMemory(&msg, sizeof(MSG));

    if (PeekMessage(&msg, wndHandle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* During PeekMessage non-queued messages are sent to 
     * the window procedure. If after this part I don't
     * check for window termination, Frame() could be called
     * on an empty WindowsUI. 
     */
    if (wndHandle == NULL) {
        return 0;
    }

    if (!Frame())
    {
        Logger::Log(L"Frame processing encountered an error.");
        PostQuitMessage(1);
        return 1;
    }

    return 0;
}

bool WindowsUI::SetFullscreen(const bool & setToFullScreen)
{
    DWORD wndStyle = 0;
    LONG_PTR result;

    if (setToFullScreen && !isFullscreen) {
        wndStyle |= WS_POPUP;
        result = SetWindowLongPtr(wndHandle, GWL_STYLE, wndStyle);

        if (!result) {
            Logger::LogWindowsErrorCode(L"setting window style to fullscreen");
            return false;
        }

        totalPixelWidth = insidePixelWidth = GetSystemMetrics(SM_CXSCREEN);
        totalPixelHeight = insidePixelHeight = GetSystemMetrics(SM_CYSCREEN);

        result = SetWindowPos(wndHandle, HWND_TOPMOST, 0, 0, totalPixelWidth, totalPixelHeight, SWP_SHOWWINDOW);

        if (!result) {
            Logger::LogWindowsErrorCode(L"setting window position to fullscreen");
            return false;
        }

        isFullscreen = true;
        return true;
    }

    if (!setToFullScreen && isFullscreen) {
        RECT rc;

        wndStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
        result = SetWindowLongPtr(wndHandle, GWL_STYLE, wndStyle);

        if (!result) {
            Logger::LogWindowsErrorCode(L"setting window style to windowed");
            return false;
        }

        insidePixelWidth = defaultPixelWidth;
        insidePixelHeight = defaultPixelHeight;

        SetRect(&rc, 0, 0, insidePixelWidth, insidePixelHeight);
        AdjustWindowRect(&rc, wndStyle, false);

        totalPixelWidth = rc.right - rc.left;
        totalPixelHeight = rc.bottom - rc.top;

        int xPos = (GetSystemMetrics(SM_CXSCREEN) - totalPixelWidth) / 2;
        int yPos = (GetSystemMetrics(SM_CYSCREEN) - totalPixelHeight) / 2;

        result = SetWindowPos(wndHandle, HWND_TOPMOST, xPos, yPos, totalPixelWidth, totalPixelHeight, SWP_SHOWWINDOW);

        if (!result) {
            Logger::LogWindowsErrorCode(L"setting window position to windowed");
            return false;
        }

        isFullscreen = false;
        return true;
    }

    return true;
}

bool WindowsUI::WindowExists()
{
    return wndHandle != NULL;
}

LRESULT WindowsUI::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
        {
            Shutdown();

            if (nWindows == 0) {
                PostQuitMessage(0);
                return 0;
            }
        }

        default:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
}

LRESULT WindowsUI::MsgForwarderSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE) {
        const CREATESTRUCTW* const createData = reinterpret_cast<CREATESTRUCTW*>(lParam);
        WindowsUI* const currentWindow = static_cast<WindowsUI*>(createData->lpCreateParams);

        SetLastError(0);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(currentWindow));
        if (GetLastError() != 0) {
            Logger::LogWindowsErrorCode(L"inserting current window pointer in window handle");
            PostQuitMessage(1);
            return 1;
        }

        SetLastError(0);
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowsUI::MsgForwarder));
        if (GetLastError() != 0) {
            Logger::LogWindowsErrorCode(L"changing window procedure to the actual forwarder");
            PostQuitMessage(1);
            return 1;
        }

        return currentWindow->MessageHandler(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WindowsUI::MsgForwarder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR result = GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (!result) {
        Logger::LogWindowsErrorCode(L"retrieving current window pointer from window handle");
        PostQuitMessage(1);
        return 1;
    }

    WindowsUI* const currentWindow = reinterpret_cast<WindowsUI*>(result);

    return currentWindow->MessageHandler(hWnd, uMsg, wParam, lParam);
}

bool WindowsUI::Frame()
{
    return childGraphics->Frame();
}

bool WindowsUI::InitializeWindow(const bool & setToFullScreen)
{
    int posX, posY;
    DWORD wndStyle = 0;

    if (nWindows == 0) {
        bool registered = RegisterWndClass();

        if (!registered) {
            Logger::Log(L"Could not register window class.");
            return false;
        }
    }

    if (setToFullScreen)
    {
        totalPixelWidth = insidePixelWidth = GetSystemMetrics(SM_CXSCREEN);
        totalPixelHeight = insidePixelHeight = GetSystemMetrics(SM_CYSCREEN);

        posX = posY = 0;

        wndStyle |= WS_POPUP;

        isFullscreen = true;
    }
    else
    {
        RECT rc;

        wndStyle |= WS_SYSMENU | WS_MINIMIZEBOX;

        insidePixelWidth = defaultPixelWidth;
        insidePixelHeight = defaultPixelHeight;

        SetRect(&rc, 0, 0, insidePixelWidth, insidePixelHeight);
        AdjustWindowRect(&rc, wndStyle, false);
        totalPixelWidth = rc.right - rc.left;
        totalPixelHeight = rc.bottom - rc.top;

        posX = (GetSystemMetrics(SM_CXSCREEN) - totalPixelWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - totalPixelHeight) / 2;

        isFullscreen = false;
    }

    wndHandle = CreateWindowEx(
        WS_EX_APPWINDOW, 
        wndClassName, 
        wndName,
        wndStyle,
        posX,
        posY,
        totalPixelWidth,
        totalPixelHeight,
        NULL,
        NULL,
        appInstance,
        this
    );

    if (wndHandle) {
        nWindows++;
    }
    else {
        Logger::LogWindowsErrorCode(L"creating window " + *wndName);

        if (nWindows == 0) {
            UnregisterWndClass();
        }

        return false;
    }
    
    ShowWindow(wndHandle, SW_SHOW);
    SetForegroundWindow(wndHandle);

    if (!SetFocus(wndHandle)) {
        Logger::LogWindowsErrorCode(L"setting focus to window " + *wndName);
        return false;
    }

    return true;
}

bool WindowsUI::RegisterWndClass()
{
    WNDCLASSEX wc;

    appInstance = GetModuleHandle(NULL);
    if (!appInstance) {
        Logger::LogWindowsErrorCode(L"getting module instance");
        return false;
    }

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = MsgForwarderSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = appInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = wndClassName;
    wc.cbSize = sizeof(WNDCLASSEX);

    if (!RegisterClassEx(&wc)) {
        Logger::LogWindowsErrorCode(L"registering class");
        return false;
    }

    return true;
}

void WindowsUI::UnregisterWndClass()
{
    UnregisterClass(wndClassName, appInstance);
    appInstance = NULL;
}
