#pragma once

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class VertexGeometry {
public:
    VertexGeometry();
    VertexGeometry(const VertexGeometry&) = delete;
    VertexGeometry& operator=(const VertexGeometry&) = delete;
    ~VertexGeometry();

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* context);
    int GetIndexCount();

private:
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    ID3D11Buffer *vertexBuffer, *indexBuffer;
    int vertexCount, indexCount;

    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* context);

};
