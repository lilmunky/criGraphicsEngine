#include "thin_windows.h"
#include "logger.h"
#include <string>
#include "shadercontroller.h"

ShaderController::ShaderController()
{
    vertexShader = 0;
    pixelShader = 0;
    layout = 0;
    matrixBuffer = 0;
}

ShaderController::~ShaderController()
{
}

bool ShaderController::Initialize(ID3D11Device * device, HWND windowHandle)
{
    return InitializeShader(device, windowHandle, L".\\bin\\color.vs", L".\\bin\\color.ps");
}

void ShaderController::Shutdown()
{
    ShutdownShader();
}

bool ShaderController::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix)
{
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projMatrix))
    {
        Logger::Log(L"Could not set shader parameters.");
        return false;
    }

    RenderShader(deviceContext, indexCount);

    return true;
}

bool ShaderController::InitializeShader(ID3D11Device * device, HWND windowHandle, const WCHAR * vsFilename, const WCHAR * psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = NULL;
    ID3D10Blob* vertexShaderBuffer = NULL;
    ID3D10Blob* pixelShaderBuffer = NULL;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    result = D3DCompileFromFile(
        vsFilename,
        NULL,
        NULL,
        "ColorVertexShader",
        "vs_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS,
        0,
        &vertexShaderBuffer,
        &errorMessage
    );
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"compiling " + wstring(vsFilename) + L" shader file", result);
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, windowHandle, vsFilename);
        }

        return false;
    }

    result = D3DCompileFromFile(
        psFilename,
        NULL,
        NULL,
        "ColorPixelShader",
        "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS,
        0,
        &pixelShaderBuffer,
        &errorMessage
    );
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"compiling " + wstring(psFilename) + L" shader file", result);
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, windowHandle, psFilename);
        }

        return false;
    }

    result = device->CreateVertexShader(
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        NULL,
        &vertexShader
    );
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating vertex shader", result);
        return false;
    }

    result = device->CreatePixelShader(
        pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(),
        NULL,
        &pixelShader
    );
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating pixel shader", result);
        return false;
    }

    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(
        polygonLayout,
        numElements,
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &layout
    );
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating input layout", result);
        return false;
    }

    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating constant buffer pointer for vertex shader", result);
        return false;
    }

    return true;
}

void ShaderController::ShutdownShader()
{
    if (matrixBuffer)
    {
        matrixBuffer->Release();
        matrixBuffer = 0;
    }

    if (layout)
    {
        layout->Release();
        layout = 0;
    }

    if (pixelShader)
    {
        pixelShader->Release();
        pixelShader = 0;
    }

    if (vertexShader)
    {
        vertexShader->Release();
        vertexShader = 0;
    }
}

void ShaderController::OutputShaderErrorMessage(ID3D10Blob * errorMsg, HWND windowHandle, const WCHAR * shaderFilename)
{
    char *compileErrors;
    wchar_t *nullTerminatedError;
    unsigned long long bufferSize, i;

    compileErrors = (char*)(errorMsg->GetBufferPointer());

    bufferSize = errorMsg->GetBufferSize();

    nullTerminatedError = new wchar_t[bufferSize + 1];

    for (i = 0; i < bufferSize; i++) {
        MultiByteToWideChar(CP_ACP, 0, &compileErrors[i], 1, &nullTerminatedError[i], 1);
    }
    nullTerminatedError[bufferSize] = L'\0';

    Logger::Log(L"Shader error: " + wstring(nullTerminatedError));

    delete[] nullTerminatedError;
    errorMsg->Release();
    errorMsg = 0;
}

bool ShaderController::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBuffer* dataPtr;
    unsigned int bufferNumber;

    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projMatrix = XMMatrixTranspose(projMatrix);

    result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"locking the constant buffer", result);
        return false;
    }

    dataPtr = (MatrixBuffer*)mappedResource.pData;

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projMatrix;

    deviceContext->Unmap(matrixBuffer, 0);

    bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

    return true;
}

void ShaderController::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(layout);

    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}
