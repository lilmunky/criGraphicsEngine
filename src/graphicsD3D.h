#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class GraphicsController;

class GraphicsD3D {
public:
    GraphicsD3D();
    GraphicsD3D(const GraphicsD3D&) = delete;
    GraphicsD3D& operator=(const GraphicsD3D&) = delete;
    ~GraphicsD3D();

    bool Initialize(GraphicsController* graphics, int screenWidth, int screenHeight, bool vsync, HWND hwnd, float screenDepth, float screenNear);
    void Shutdown();

    void BeginScene(float red, float green, float blue, float alpha);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX& projMatrix);
    void GetWorldMatrix(XMMATRIX& wMatrix);
    void GetOrthoMatrix(XMMATRIX& ortMatrix);

    void GetVideoCardInfo(char* cardName, int& memory);

private:
    int videoCardMemory;
    char videoCardDescription[128];
    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilState* depthStencilState;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11RasterizerState* rasterState;
    XMMATRIX projectionMatrix;
    XMMATRIX worldMatrix;
    XMMATRIX orthoMatrix;
    GraphicsController* parentGraphics;
};
