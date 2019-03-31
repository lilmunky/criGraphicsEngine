#include "thin_windows.h"
#include "logger.h"
#include "graphicscontroller.h"
#include "graphicsd3d.h"
#include <string>

GraphicsD3D::GraphicsD3D()
{
    swapChain = 0;
    device = 0;
    deviceContext = 0;
    renderTargetView = 0;
    depthStencilBuffer = 0;
    depthStencilState = 0;
    depthStencilView = 0;
    rasterState = 0;
    parentGraphics = 0;
}

GraphicsD3D::~GraphicsD3D()
{
}

bool GraphicsD3D::Initialize(GraphicsController * graphics, int screenWidth, int screenHeight, bool vsync, HWND hwnd, float screenDepth, float screenNear)
{
    if (!graphics) {
        Logger::Log(L"Graphics contoller pointer is invalid for Direct3D object.");
        return false;
    }

    DXGI_FORMAT displayFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator;
    unsigned long long stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBuffer;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_VIEWPORT viewport;
    float fieldOfView, screenAspect;

    parentGraphics = graphics;

    /* Refresh rate retrieval */
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating DirectX graphics interface factory", result);
        return false;
    }

    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating adapter for graphics interface", result);
        return false;
    }

    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"enumerating primary adapter output", result);
        return false;
    }

    result = adapterOutput->GetDisplayModeList(displayFormat, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"retrieving the modes that fit the display format for the adapter output (" + to_wstring(displayFormat) + L")", result);
        return false;
    }

    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        Logger::LogD3DErrorCode(L"creating diplay modes array", result);
        return false;
    }

    result = adapterOutput->GetDisplayModeList(displayFormat, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"filling display modes array", result);
        return false;
    }

    for (i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"getting the adapter description", result);
        return false;
    }

    videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
    if (error)
    {
        wchar_t *errBuffer = new wchar_t[256];
        _wcserror_s(errBuffer, 200, errno);
        wstring errStr(errBuffer);
        Logger::Log(L"Error getting adapter name:" + errStr);
        delete errBuffer;
        return false;
    }

    delete[] displayModeList;
    displayModeList = 0;

    adapterOutput->Release();
    adapterOutput = 0;

    adapter->Release();
    adapter = 0;

    factory->Release();
    factory = 0;

    /* Swap chain, device, and context setup */
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    if (parentGraphics->vsync)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    result = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain,
        &device,
        NULL,
        &deviceContext
    );
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating Direct3D device and swapchain", result);
        return false;
    }

    result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if(FAILED(result))
    {
        Logger::LogD3DErrorCode(L"getting the pointer to the back buffer", result);
        return false;
    }

    result = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
    if(FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating the render target view", result);
        return false;
    }

    backBuffer->Release();
    backBuffer = 0;

    /* Depth buffer and associated stencil setup */
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating 2d texture", result);
        return false;
    }

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating depth stencil state", result);
        return false;
    }
    deviceContext->OMSetDepthStencilState(depthStencilState, 1);

    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating depth stencil view", result);
        return false;
    }
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    /* Rasterizer setup */
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    result = device->CreateRasterizerState(&rasterDesc, &rasterState);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating rasterizer state (default rasterizer state)", result);
        return false;
    }
    deviceContext->RSSetState(rasterState);

    /* Viewport for rendering setup */
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    deviceContext->RSSetViewports(1, &viewport);

    /* Matrices setup */
    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    worldMatrix = XMMatrixIdentity();

    orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}

void GraphicsD3D::Shutdown()
{
    if (swapChain)
    {
        swapChain->SetFullscreenState(false, NULL);
    }

    if (rasterState)
    {
        rasterState->Release();
        rasterState = 0;
    }

    if (depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = 0;
    }

    if (depthStencilState)
    {
        depthStencilState->Release();
        depthStencilState = 0;
    }

    if (depthStencilBuffer)
    {
        depthStencilBuffer->Release();
        depthStencilBuffer = 0;
    }

    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = 0;
    }

    if (deviceContext)
    {
        deviceContext->Release();
        deviceContext = 0;
    }

    if (device)
    {
        device->Release();
        device = 0;
    }

    if (swapChain)
    {
        swapChain->Release();
        swapChain = 0;
    }
}

void GraphicsD3D::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    deviceContext->ClearRenderTargetView(renderTargetView, color);

    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GraphicsD3D::EndScene()
{
    if (parentGraphics->vsync)
    {
        swapChain->Present(1, 0);
    }
    else
    {
        swapChain->Present(0, 0);
    }
}

ID3D11Device * GraphicsD3D::GetDevice()
{
    return device;
}

ID3D11DeviceContext * GraphicsD3D::GetDeviceContext()
{
    return deviceContext;
}

void GraphicsD3D::GetProjectionMatrix(XMMATRIX & projMatrix)
{
    projMatrix = projectionMatrix;
}

void GraphicsD3D::GetWorldMatrix(XMMATRIX & wMatrix)
{
    wMatrix = worldMatrix;
}

void GraphicsD3D::GetOrthoMatrix(XMMATRIX & ortMatrix)
{
    ortMatrix = orthoMatrix;
}

void GraphicsD3D::GetVideoCardInfo(char * cardName, int & memory)
{
    strcpy_s(cardName, 128, videoCardDescription);
    memory = videoCardMemory;
}
