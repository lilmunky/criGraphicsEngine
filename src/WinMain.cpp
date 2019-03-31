#include "thin_windows.h"
#include "logger.h"
#include "windowsUI.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    Logger::InitLog();
    WindowsUI* windowsUI;
    bool wndCreated;
    int exitCode = 0;

    windowsUI = new WindowsUI;
    if (!windowsUI)
    {
        Logger::Log(L"Could not create new Windows GUI object");
        return 1;
    }

    wndCreated = windowsUI->NewWindow(L"CriEngine", false);
    
    if (!wndCreated) {
        Logger::Log(L"Could not complete window creation.");

        windowsUI->Shutdown();
        delete windowsUI;

        return 1;
    }

    while (!exitCode && windowsUI->WindowExists()) {
        exitCode = windowsUI->Run();
    }

    windowsUI->Shutdown();
    delete windowsUI;

    return exitCode;
}
