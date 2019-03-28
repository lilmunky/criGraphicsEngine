#include "stdafx.h"
#include "windowsUI.h"
#include "logger.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    Logger::InitLog();
    //TODO: find good implementation for crossplatform support
    WindowsUI *userInterface = new WindowsUI;

    int retValue = userInterface->UIMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    delete(userInterface);

    return retValue;
}
