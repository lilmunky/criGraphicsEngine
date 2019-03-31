#pragma once

class WindowsUI;
class GraphicsD3D;

class GraphicsController {
    friend GraphicsD3D;

public:
    GraphicsController();
    GraphicsController(const GraphicsController&) = delete;
    GraphicsController& operator=(const GraphicsController&) = delete;
    ~GraphicsController();

    bool Initialize(WindowsUI* const wnd, bool vSync = false, float scDepth = 1000.0f, float scNear = 0.1f);
    void Shutdown();
    bool Frame();

private:
    bool vsync;
    float screen_depth;
    float screen_near;
    WindowsUI *parentWindow;
    GraphicsD3D *childGd3d;

    bool Render();
};
