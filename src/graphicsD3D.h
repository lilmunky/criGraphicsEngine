#pragma once

class WindowsUI;

class GraphicsD3D {
public:
    GraphicsD3D();
    GraphicsD3D(const GraphicsD3D&) = delete;
    ~GraphicsD3D();

    bool Initialize();
    void ShutDown();
    bool Frame();
private:
    WindowsUI *window;
    bool vsync;
    float screen_depth;
    float screen_near;

    bool Render();
};
