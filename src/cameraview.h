#pragma once

#include <directxmath.h>
using namespace DirectX;

class CameraView {
public:
    CameraView();
    CameraView(const CameraView&) = delete;
    CameraView& operator=(const CameraView&) = delete;
    ~CameraView();

    void SetPosition(float x, float y, float z);
    void SetRotation(float x, float y, float z);

    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();

    void Render();
    void GetViewMatrix(XMMATRIX& viewMatrix);

private:
    float positionX, positionY, positionZ;
    float rotationX, rotationY, rotationZ;
    XMMATRIX viewMatrix;
};
