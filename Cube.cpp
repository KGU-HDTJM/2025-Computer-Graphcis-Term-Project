#include "Cube.h"
#include <iostream>

using namespace DirectX;

Cube::Cube(ID3D11Device* _Device, ID3D11DeviceContext* _ImmediateContext, float x, float y, float z, float size)
    : mDevice(_Device), mImmediateContext(_ImmediateContext), mIndexBuffer(nullptr), mVertexBuffer(nullptr)
{
    createGeometry(x, y, z, size);
    if (!createBuffers())
    {
        MessageBoxA(nullptr, "Failed to create cube index and vertex buffer", "Error", MB_OK);
        assert(false);
    }
}

Cube::~Cube()
{
    if (mVertexBuffer) { mVertexBuffer->Release(); }
    if (mIndexBuffer) { mIndexBuffer->Release(); }
}

size_t Cube::GetIndexCount(void) const { return mIndices.size(); }
ID3D11Buffer* Cube::GetIndexBuffer(void) const { return mIndexBuffer; }
ID3D11Buffer* Cube::GetVertexBuffer(void) const { return mVertexBuffer; }
size_t Cube::GetVertexSize(void) const { return sizeof(Vertex); }


void Cube::createGeometry(float x, float y, float z, float size)
{
    float halfSize = size * 0.5f;
    float cx = x;
    float cy = y;
    float cz = z;

    mVertices.resize(8);
    mVertices[0] = { {cx - halfSize, cy - halfSize, cz - halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    mVertices[1] = { {cx + halfSize, cy - halfSize, cz - halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    mVertices[2] = { {cx + halfSize, cy + halfSize, cz - halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    mVertices[3] = { {cx - halfSize, cy + halfSize, cz - halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };

    mVertices[4] = { {cx - halfSize, cy - halfSize, cz + halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    mVertices[5] = { {cx + halfSize, cy - halfSize, cz + halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    mVertices[6] = { {cx + halfSize, cy + halfSize, cz + halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    mVertices[7] = { {cx - halfSize, cy + halfSize, cz + halfSize, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    
    mIndices.resize(36);
    mIndices[0] = 4; mIndices[1] = 5; mIndices[2] = 6;
    mIndices[3] = 4; mIndices[4] = 6; mIndices[5] = 7;
    mIndices[6] = 1; mIndices[7] = 0; mIndices[8] = 3;
    mIndices[9] = 1; mIndices[10] = 3; mIndices[11] = 2;
    mIndices[12] = 5; mIndices[13] = 1; mIndices[14] = 2;
    mIndices[15] = 5; mIndices[16] = 2; mIndices[17] = 6;
    mIndices[18] = 0; mIndices[19] = 4; mIndices[20] = 7;
    mIndices[21] = 0; mIndices[22] = 7; mIndices[23] = 3;
    mIndices[24] = 3; mIndices[25] = 7; mIndices[26] = 6;
    mIndices[27] = 3; mIndices[28] = 6; mIndices[29] = 2;
    mIndices[30] = 4; mIndices[31] = 0; mIndices[32] = 1;
    mIndices[33] = 4; mIndices[34] = 1; mIndices[35] = 5;
}

bool Cube::createBuffers()
{
    HRESULT hr;

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));

    bufferDesc.ByteWidth = sizeof(Vertex) * mVertices.size();
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = mVertices.data();

    hr = mDevice->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
    if (FAILED(hr))
    {
        goto LB_FAILED_CREATE_VERTEX_BUFFER;
    }

    bufferDesc.ByteWidth = sizeof(uint32_t) * mIndices.size();
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = mIndices.data();

    hr = mDevice->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
    if (FAILED(hr))
    {
        goto LB_FAILED_CREATE_INDEX_BUFFER;
    }

    return true;

LB_FAILED_CREATE_INDEX_BUFFER:
    if (mVertexBuffer) mVertexBuffer->Release();
LB_FAILED_CREATE_VERTEX_BUFFER:
    return false;
}