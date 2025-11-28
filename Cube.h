#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <EASTL/vector.h>

using namespace DirectX;
using namespace eastl;

struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT4 nor; 
};

class Cube
{
public:
	Cube(void) : mDevice(nullptr), mImmediateContext(nullptr), mIndexBuffer(nullptr), mVertexBuffer(nullptr) {}
	Cube(ID3D11Device* _Device, ID3D11DeviceContext* _ImmediateContext, float x, float y, float z, float size);
	~Cube();
	
	size_t GetIndexCount(void) const;
	ID3D11Buffer* GetIndexBuffer(void) const;
	ID3D11Buffer* GetVertexBuffer(void) const;
	size_t GetVertexSize(void) const;

private:
	void createGeometry(float x, float y, float z, float size);
	bool createBuffers();

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mImmediateContext;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	vector<Vertex> mVertices;
	vector<uint32_t> mIndices;
};