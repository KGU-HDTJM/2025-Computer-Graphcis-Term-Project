#pragma once

#include<d3d11.h>
#include<DirectXMath.h>
#include<EASTL/vector.h>
#include<ctime>

#include "define.h"
#include "D3D11Base.h"

class Map
{
public:

	Map(void) : mBase(nullptr), mIndexBuffer(nullptr), mVertexBuffer(nullptr){}
	Map(D3D11Base* _base, const int _x, const int _y, const int _scale) : mBase(_base)
	{
		srand(static_cast<uint32_t>(std::time(nullptr)));

		createVertex(_x, _y, _scale);
		createIndex(_x, _y, _scale);

		if (!createBuffers())
		{
			MessageBoxA(nullptr, "Failed to create index and vertex buffer", "Error", MB_OK);
			assert(false);
		}
	}

	void AddPerlinLayer(int x, int y, int scale);
	void Draw(void);


	~Map()
	{
		if (mVertexBuffer) { mVertexBuffer->Release(); }
		if (mIndexBuffer) { mIndexBuffer->Release();   }
	}

	size_t GetIndexCount(void);
	ID3D11Buffer* GetIndexBuffer(void);
	ID3D11Buffer* GetVertexBuffer(void);
	size_t GetVertexSize(void) const;

private:
	
	eastl::vector<Vertex> createVertex(const int& x, const int& y, const int& scale);
	void createIndex(const int& x, const int& y, const int& scale);

	bool createBuffers(void);
	void updateVertexBuffer(const eastl::vector<Vertex>& newPerlin);

private:

	const char* VERTEX_FILE = "perlin.vx.bin";
	const char* INDEX_FILE  = "perlin.ix.bin";
	
	D3D11Base* mBase = nullptr;

	ID3D11Buffer* mIndexBuffer  = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;

	eastl::vector<uint32_t> mIndices;
	eastl::vector<Vertex> mVertices;
};