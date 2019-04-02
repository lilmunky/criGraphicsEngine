#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ShaderController {
public:
    ShaderController();
    ShaderController(const ShaderController&) = delete;
    ShaderController& operator=(const ShaderController&) = delete;
    ~ShaderController();

    bool Initialize(ID3D11Device* device, HWND windowHandle);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix);

private:
    struct MatrixBuffer
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* layout;
    ID3D11Buffer* matrixBuffer;

    bool InitializeShader(ID3D11Device* device, HWND windowHandle, const WCHAR* vsFilename, const WCHAR* psFilename);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND windowHandle, const WCHAR* shaderFilename);
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};
