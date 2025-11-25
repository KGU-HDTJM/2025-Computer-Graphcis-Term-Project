#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")  
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include<DirectXMath.h>
#include<EASTL/vector.h>

struct Vertex
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 normal;
	DirectX::XMFLOAT2 texcoord;
};

class Sphere
{
public:

	Sphere(void): 
		 mDevice(nullptr), mImmediateContext(nullptr), mIndexCount(0)
		,mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndices(nullptr), mVertices(nullptr)
	{}

	Sphere(ID3D11Device* _Device, ID3D11DeviceContext* _ImmediateContext) : mDevice(_Device), mImmediateContext(_ImmediateContext)
	{
		setupSphere();
		if (!createMeshBuffers())
		{
			MessageBoxA(nullptr, "Failed to create index and vertex buffer", "Error", MB_OK);
			assert(false);
		}
	}

	void CleanUp(void);

	ID3D11Buffer* GetIndexBuffer(void) const;
	ID3D11Buffer* GetVertexBuffer(void) const;
	UINT GetIndexCount(void) const;

private:

	void setupSphere(void);
	bool createMeshBuffers(void);

private:

	ID3D11Device* const mDevice;
	ID3D11DeviceContext* const mImmediateContext;

	UINT mIndexCount;

	ID3D11Buffer* mIndexBuffer = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;

	eastl::vector<uint32_t> mIndices;
	eastl::vector<Vertex> mVertices;
};