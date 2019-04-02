#include "thin_windows.h"
#include "logger.h"
#include "vertexgeometry.h"

VertexGeometry::VertexGeometry()
{
    vertexBuffer = 0;
    indexBuffer = 0;
}

VertexGeometry::~VertexGeometry()
{
}

bool VertexGeometry::Initialize(ID3D11Device * device)
{
    return InitializeBuffers(device);
}

void VertexGeometry::Shutdown()
{
    ShutdownBuffers();
}

void VertexGeometry::Render(ID3D11DeviceContext * context)
{
    RenderBuffers(context);
}

int VertexGeometry::GetIndexCount()
{
    return indexCount;
}

bool VertexGeometry::InitializeBuffers(ID3D11Device * device)
{
    Vertex* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    vertexCount = 3;
    indexCount = 3;

    vertices = new Vertex[vertexCount];
    indices = new unsigned long[indexCount];

    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating vertex buffer", result);
        return false;
    }

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
    if (FAILED(result))
    {
        Logger::LogD3DErrorCode(L"creating index buffer", result);
        return false;
    }

    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

void VertexGeometry::ShutdownBuffers()
{
    if (indexBuffer)
    {
        indexBuffer->Release();
        indexBuffer = 0;
    }

    if (vertexBuffer)
    {
        vertexBuffer->Release();
        vertexBuffer = 0;
    }
}

void VertexGeometry::RenderBuffers(ID3D11DeviceContext * context)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(Vertex);
    offset = 0;

    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
