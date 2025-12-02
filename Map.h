#pragma once

#include<d3d11.h>
#include<DirectXMath.h>
#include<EASTL/vector.h>
#include<ctime>
#include<string>

#include "define.h"
#include "D3D11Base.h"

enum class eTerrain
{
	FLAT,
	BUMPY,
	JAGGED,
	COUNT
};

class Map
{
public:

	Map(void) : mBase(nullptr), mIndexBuffer(nullptr), mVertexBuffer(nullptr)
	{
		mVertices = eastl::vector<Vertex>();
		mIndices = eastl::vector<uint32_t>();
		mCamPos = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	Map(D3D11Base* _base) : mBase(_base)
	{		
		if (!loadMeshData())
		{
			MessageBoxA(nullptr, "Failed to load index and vertex", "Error", MB_OK);
			assert(false);
		}
		if (!createBuffers())
		{
			MessageBoxA(nullptr, "Failed to create index and vertex buffer", "Error", MB_OK);
			assert(false);
		}
	}

	~Map()
	{
		if (mVertexBuffer) { mVertexBuffer->Release(); }
		if (mIndexBuffer) { mIndexBuffer->Release();   }
	}

	void Draw(void);

	size_t GetIndexCount(void);
	ID3D11Buffer* GetIndexBuffer(void);
	ID3D11Buffer* GetVertexBuffer(void);
	size_t GetVertexSize(void) const;
	void UpdateCameraPos(const XMFLOAT4& pos);

private:
	
	bool loadMeshData(void);

	eastl::vector<Vertex> createVertex(const int& x, const int& z, const int& scale);

	void createIndex(const int& x, const int& z);
	bool createBuffers(void);
	
	void updateVertexBuffer(const eastl::vector<Vertex>& newPerlin);
	void updateIndexBuffer(void);

private:
	
	const char* MAIN_SEED_FILE = "perlin.seed.bin";
	
	XMFLOAT4 mCamPos;

	D3D11Base* mBase = nullptr;
	eastl::vector<uint32_t> mIndices;
	eastl::vector<Vertex> mVertices;

	ID3D11Buffer* mIndexBuffer  = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;
};