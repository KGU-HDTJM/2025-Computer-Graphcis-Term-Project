#pragma once

#include<d3d11.h>
#include<DirectXMath.h>
#include<EASTL/vector.h>
#include<ctime>

class Perlin
{
public:

	Perlin(void) : mDevice(nullptr), mImmediateContext(nullptr), mIndexBuffer(nullptr), mVertexBuffer(nullptr){}
	Perlin(ID3D11Device* _Device, ID3D11DeviceContext* _ImmediateContext, const int _x, const int _y, const int _scale) : mDevice(_Device), mImmediateContext(_ImmediateContext)
	{
		srand(static_cast<uint32_t>(std::time(nullptr)));

		createNoise(_x, _y, _scale);
		if (!createBuffers())
		{
			MessageBoxA(nullptr, "Failed to create index and vertex buffer", "Error", MB_OK);
			assert(false);
		}
	}

	~Perlin()
	{
		if (mVertexBuffer) { mVertexBuffer->Release(); }
		if (mIndexBuffer) { mIndexBuffer->Release();   }
	}

	size_t GetIndexCount(void);
	ID3D11Buffer* GetIndexBuffer(void);
	ID3D11Buffer* GetVertexBuffer(void);
	size_t GetVertexSize(void) const;

private:

	void createNoise(int x, int y, int scale);
	bool createBuffers(void);

private:
	
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mImmediateContext = nullptr;

	ID3D11Buffer* mIndexBuffer  = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;
	
	struct Vertex {
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 nor;
	};

	eastl::vector<uint32_t> mIndices;
	eastl::vector<Vertex> mVertices;
	
};