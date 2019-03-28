#include "stdafx.h"
#include "windowsUI.h"
#include "logger.h"
#include <comdef.h>
#include <sstream>

string WindowsUI::wndName = "CriEngine";
string WindowsUI::wndClassName = "EngineMainWindow";
HINSTANCE WindowsUI::t_hInstance = NULL;

LRESULT WindowsUI::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
    {
        HMENU hMenu;
        hMenu = GetMenu(hWnd);
        if (hMenu != NULL)
        {
            DestroyMenu(hMenu);
        }

        DestroyWindow(hWnd);

        UnregisterClass(
            wndClassName.c_str(),
            t_hInstance
        );

        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WindowsUI::FillWndClass()
{
    char szExePath[MAX_PATH];
    GetModuleFileName(NULL, szExePath, MAX_PATH);
    if (hIcon == NULL) {
        hIcon = ExtractIcon(t_hInstance, szExePath, 0);
    }

    wndClass.style = CS_DBLCLKS;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = t_hInstance;
    wndClass.hIcon = hIcon;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = wndClassName.c_str();
}

void WindowsUI::InitWindowCreation()
{
    if (!RegisterClass(&wndClass)) {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CLASS_ALREADY_EXISTS) {
            _com_error err(dwError);
            stringstream strErr;
            strErr << hex << dwError;
            Logger::Log("Error registering window class: 0x" + strErr.str() + " - " + err.ErrorMessage());
            abort();
        }
    }

    RECT totalWndRect;
    SetRect(&totalWndRect, 0, 0, pixelWidth, pixelHeight);
    AdjustWindowRect(&totalWndRect, WS_OVERLAPPEDWINDOW, FALSE);

    mainWindowHandler = CreateWindowEx(0,
                                       wndClassName.c_str(),
                                       wndName.c_str(),
                                       WS_OVERLAPPEDWINDOW,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       totalWndRect.right - totalWndRect.left, totalWndRect.bottom - totalWndRect.top,
                                       NULL,
                                       NULL,
                                       t_hInstance,
                                       NULL);

    if (mainWindowHandler == NULL)
    {
        DWORD dwError = GetLastError();
        _com_error err(dwError);
        stringstream strErr;
        strErr << hex << dwError;
        Logger::Log("Error creating window: 0x" + strErr.str() + " - " + err.ErrorMessage());
        abort();
    }


}

int WindowsUI::MainLoop()
{
    bool gotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

    while (WM_QUIT != msg.message)
    {
        gotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

        if (gotMsg)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            //// Update the scene.
            //renderer->Update();

            //// Render frames during idle time (when no messages are waiting).
            //renderer->Render();

            //// Present the frame to the screen.
            //deviceResources->Present();
        }
    }
    return 0;
}

WindowsUI::WindowsUI() {

}

WindowsUI::~WindowsUI() {

}

int WindowsUI::UIMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    t_hInstance = hInstance;

    FillWndClass();

    InitWindowCreation();

    ShowWindow(mainWindowHandler, SW_SHOW);

    MainLoop();

    return 0;
}
