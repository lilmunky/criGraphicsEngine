#include "thin_windows.h"
#include "logger.h"
#include "windowsUI.h"
#include "graphicsd3d.h"
#include "cameraview.h"
#include "vertexgeometry.h"
#include "shadercontroller.h"
#include "graphicscontroller.h"

GraphicsController::GraphicsController()
{
    parentWindow = NULL;
    childGd3d = NULL;
    childCamera = NULL;
    childShaderController = NULL;
    childVertexGeom = NULL;
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

    childCamera = new CameraView;
    childCamera->SetPosition(0.0f, 0.0f, -5.0f);

    childVertexGeom = new VertexGeometry;
    result = childVertexGeom->Initialize(childGd3d->GetDevice());
    if (!result) {
        Logger::Log(L"Could not initialize vertex geometry controller.");
        return false;
    }

    childShaderController = new ShaderController;
    result = childShaderController->Initialize(childGd3d->GetDevice(), wnd->wndHandle);
    if (!result) {
        Logger::Log(L"Could not initialize shader controller.");
        return false;
    }

    return true;
}

void GraphicsController::Shutdown()
{
    if (childCamera) {
        delete childCamera;
        childCamera = NULL;
    }

    if (childVertexGeom) {
        childVertexGeom->Shutdown();
        delete childVertexGeom;
        childVertexGeom = NULL;
    }

    if (childShaderController) {
        childShaderController->Shutdown();
        delete childShaderController;
        childShaderController = NULL;
    }

    if (childGd3d) {
        childGd3d->Shutdown();
        delete childGd3d;
        childGd3d = NULL;
    }
}

bool GraphicsController::Frame()
{
    return Render();
}

bool GraphicsController::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    childGd3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    childCamera->Render();

    childGd3d->GetWorldMatrix(worldMatrix);
    childCamera->GetViewMatrix(viewMatrix);
    childGd3d->GetProjectionMatrix(projectionMatrix);

    childVertexGeom->Render(childGd3d->GetDeviceContext());

    result = childShaderController->Render(childGd3d->GetDeviceContext(), childVertexGeom->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    if (!result)
    {
        Logger::Log(L"Could not render matrices through the shader controller.");
        return false;
    }

    childGd3d->EndScene();

    return true;
}
