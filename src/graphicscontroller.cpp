#include "thin_windows.h"
#include "logger.h"
#include "windowsUI.h"
#include "graphicsd3d.h"
#include "graphicscontroller.h"

GraphicsController::GraphicsController()
{
    parentWindow = 0;
    childGd3d = 0;
}

GraphicsController::~GraphicsController()
{
}

bool GraphicsController::Initialize(WindowsUI* const wnd, bool vSync, float scDepth, float scNear) {
    if (!wnd) {
        Logger::Log(L"The window pointer for graphics initialization is void.");
        return false;
    }

    if (!wnd->wndHandle) {
        Logger::Log(L"No window found to attach graphics to.");
        return false;
    }

    bool result;

    parentWindow = wnd;
    vsync = vSync;
    screen_depth = scDepth;
    screen_near = scNear;

    childGd3d = new GraphicsD3D;

    result = childGd3d->Initialize(
        reinterpret_cast<GraphicsController*>(this),
        parentWindow->insidePixelWidth,
        parentWindow->insidePixelHeight,
        vsync,
        parentWindow->wndHandle,
        screen_depth,
        screen_near
    );
    if (!result) {
        Logger::Log(L"Could not initialize Direct3D object.");
        return false;
    }

    return true;
}

void GraphicsController::Shutdown()
{
    if (childGd3d) {
        childGd3d->Shutdown();
        delete childGd3d;
        childGd3d = 0;
    }
}

bool GraphicsController::Frame()
{
    return Render();
}

bool GraphicsController::Render()
{
    childGd3d->BeginScene(0.8f, 0.3f, 0.6f, 1.0f);

    childGd3d->EndScene();

    return true;
}
