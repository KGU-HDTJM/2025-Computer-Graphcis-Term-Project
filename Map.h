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

	Map(void) : mBase(nullptr), mVertexBuffer(nullptr)
	{
		mIndexBuffers = eastl::vector<ID3D11Buffer*>();
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
		for (auto i = mIndexBuffers.begin(); i != mIndexBuffers.end(); ++i)
		{
			(*i)->Release();
		}
	}

	void Draw(void);
	void UpdateCameraPos(const XMFLOAT4& pos);

private:
	
	bool loadMeshData(void);

	eastl::vector<Vertex> createVertex(const int& x, const int& z, const float& scale);
	
	void createIndex(const int& x, const int& z);
	bool createBuffers(void);

	void updateVertexBuffer(const eastl::vector<Vertex>& newPerlin, const int& x, const int& z);
	void updateChunkIndexBuffers(void);
public:
	const int MAP_DIM = 1000;
	const int CHUNK_DIM = 100;
	const int TERRAIN_COUNT = 4;
	const int INDEX_BUFFER_DIM = MAP_DIM / CHUNK_DIM;
private:
	const char* MAIN_SEED_FILE = "perlin.seed.bin";

	

	XMFLOAT4 mCamPos;

	D3D11Base* mBase = nullptr;
	eastl::vector<uint32_t> mIndices;
	eastl::vector<Vertex> mVertices;

	ID3D11Buffer* mVertexBuffer = nullptr;
	eastl::vector<ID3D11Buffer*> mIndexBuffers;
};