#include "thin_windows.h"
#include "windowsUI.h"
#include "logger.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    Logger::InitLog();
    //TODO: find good implementation for crossplatform support
    WindowsUI* windowsUI;
    bool result;

    windowsUI = new WindowsUI;
    if (!windowsUI)
    {
        return 0;
    }

    result = windowsUI->Initialize();
    if (result)
    {
        windowsUI->Run();
    }

    windowsUI->Shutdown();
    delete windowsUI;

    return 0;
}
