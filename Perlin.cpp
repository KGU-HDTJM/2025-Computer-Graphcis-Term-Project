#include"Perlin.h"

#include<cstdlib>
#include<iostream>

using namespace DirectX;

const float PI = acosf(-1);

inline float lerp(float v0, float v1, float t)
{
	return v0 + t * (v1 - v0);
}

inline float fade(float t)
{
	return ((6 * t - 15) * t + 10) * t * t * t;
}

size_t Perlin::GetIndexCount(void)
{
	return mIndices.size();
}

ID3D11Buffer* Perlin::GetIndexBuffer(void)
{
	return mIndexBuffer;
}

ID3D11Buffer* Perlin::GetVertexBuffer(void)
{
	return mVertexBuffer;
}

size_t Perlin::GetVertexSize(void) const 
{
	return sizeof(Vertex);
}


void Perlin::createNoise(int x, int y, int scale)
{
	// Clear any existing data
	mVertices.clear();
	mIndices.clear();

	eastl::vector<float> randDir;

	// generate random directions for grid corners (coarse grid scaled by 'scale')
	const int COARSE_W = x * 2 * scale;
	const int COARSE_H = y * 2 * scale;
	randDir.reserve(COARSE_W * COARSE_H);

	for (int i = 0; i < COARSE_W * COARSE_H; ++i)
	{
		float angle = (rand() % 360) * PI / 180.0f;
		randDir.push_back(angle);
	}

	const int GRID_WIDTH = x * scale;
	const int GRID_HEIGHT = y * scale;

	// store heights first
	eastl::vector<float> heights;
	heights.resize(GRID_WIDTH * GRID_HEIGHT);

	for (int j = 0; j < GRID_HEIGHT; ++j)
	{
		for (int i = 0; i < GRID_WIDTH; ++i)
		{
			const float px = (float)i / (float)(GRID_WIDTH);
			const float py = (float)j / (float)(GRID_HEIGHT);
			float standX = px - floorf(px);
			float standY = py - floorf(py);

			// Map to indices into randDir: replicate earlier indexing intent but clipped to available data
			int baseX = (i * 2) % COARSE_W;
			int baseY = (j * 2) % COARSE_H;

			int i00 = baseX + COARSE_W * baseY;
			int i10 = (baseX + 1) + COARSE_W * baseY;
			int i01 = baseX + COARSE_W * (baseY + 1 < COARSE_H ? baseY + 1 : baseY);
			int i11 = (baseX + 1) + COARSE_W * (baseY + 1 < COARSE_H ? baseY + 1 : baseY);

			float p00 = (0 - standX) * cosf(randDir[i00]) + (0 - standY) * sinf(randDir[i00]);
			float p10 = (1 - standX) * cosf(randDir[i10]) + (0 - standY) * sinf(randDir[i10]);
			float p01 = (0 - standX) * cosf(randDir[i01]) + (1 - standY) * sinf(randDir[i01]);
			float p11 = (1 - standX) * cosf(randDir[i11]) + (1 - standY) * sinf(randDir[i11]);

			float u = fade(standX);
			float v = fade(standY);

			float n0 = lerp(p00, p01, v);
			float n1 = lerp(p10, p11, v);
			float result = lerp(n0, n1, u);

			// adjust height scale as before
			float adjustHeight = (result + 1.0f) * 0.5f * 10.0f + 1.0f;

			heights[i + GRID_WIDTH * j] = adjustHeight;
		}
	}

	// create vertices with normals computed by central differences
	for (int j = 0; j < GRID_HEIGHT; ++j)
	{
		for (int i = 0; i < GRID_WIDTH; ++i)
		{
			Vertex node;
			float h = heights[i + GRID_WIDTH * j];
			node.pos = { static_cast<float>(i), h, static_cast<float>(j), 1.0f };

			// compute derivatives gx = dh/dx, gz = dh/dz using central differences
			float gx, gz;

			if (i == 0)
				gx = heights[(i + 1) + GRID_WIDTH * j] - heights[i + GRID_WIDTH * j];
			else if (i == GRID_WIDTH - 1)
				gx = heights[i + GRID_WIDTH * j] - heights[(i - 1) + GRID_WIDTH * j];
			else
				gx = (heights[(i + 1) + GRID_WIDTH * j] - heights[(i - 1) + GRID_WIDTH * j]) * 0.5f;

			if (j == 0)
				gz = heights[i + GRID_WIDTH * (j + 1)] - heights[i + GRID_WIDTH * j];
			else if (j == GRID_HEIGHT - 1)
				gz = heights[i + GRID_WIDTH * j] - heights[i + GRID_WIDTH * (j - 1)];
			else
				gz = (heights[i + GRID_WIDTH * (j + 1)] - heights[i + GRID_WIDTH * (j - 1)]) * 0.5f;

			// normal approximation: (-gx, 1.0, -gz) then normalize. Use a scale factor on the y to control steepness
			XMVECTOR n = XMVectorSet(-gx, 2.0f, -gz, 0.0f);
			n = XMVector3Normalize(n);
			XMFLOAT3 nf;
			XMStoreFloat3(&nf, n);
			node.nor = { nf.x, nf.y, nf.z, 0.0f };

			mVertices.push_back(node);
		}
	}

	// indices (triangle list) same as before
	for (int j = 0; j < GRID_HEIGHT - 1; ++j)
	{
		for (int i = 0; i < GRID_WIDTH - 1; ++i)
		{
			int p00 = i + GRID_WIDTH * j;
			int p01 = i + 1 + GRID_WIDTH * j;
			int p10 = i + GRID_WIDTH * (j + 1);
			int p11 = i + 1 + GRID_WIDTH * (j + 1);

			mIndices.push_back(static_cast<uint32_t>(p00));
			mIndices.push_back(static_cast<uint32_t>(p11));
			mIndices.push_back(static_cast<uint32_t>(p01));

			mIndices.push_back(static_cast<uint32_t>(p00));
			mIndices.push_back(static_cast<uint32_t>(p10));
			mIndices.push_back(static_cast<uint32_t>(p11));
		}
	}
}

bool Perlin::createBuffers(void)
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
	mVertexBuffer->Release();
LB_FAILED_CREATE_VERTEX_BUFFER:

	return false;
}