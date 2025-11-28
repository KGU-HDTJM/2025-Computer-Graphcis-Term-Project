#include"Perlin.h"

#include<ctime>
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
	eastl::vector<float> randDir;


	for (int i = 0; i < y * 2 * scale * x * 2 * scale; ++i)
	{
		float angle = rand() % 360 * PI / 180.0f;
		randDir.push_back(angle);
	}

	const int SCALED_Y = y * 2 * scale;
	const int SCALED_X = x * 2 * scale;

	for (int j = 0; j < y * scale; ++j)
	{
		for (int i = 0; i < x * scale; ++i)
		{
			const float px = (float)i / (float)(x * scale);
			const float py = (float)j / (float)(y * scale);
			float standX = px - floorf(px);
			float standY = py - floorf(py);

			int i00 = i * 2 +	  SCALED_X * j * 2;
			int i01 = i * 2 +	  SCALED_X * (j * 2+ 1);
			int i10 = i * 2 + 1 + SCALED_X * j * 2;
			int i11 = i * 2 + 1 + SCALED_X * (j * 2 + 1);

			float p00 = (0 - standX) * cosf(randDir[i00]) + (0 - standY) * sinf(randDir[i00]);
			float p10 = (1 - standX) * cosf(randDir[i10]) + (0 - standY) * sinf(randDir[i10]);
			float p01 = (0 - standX) * cosf(randDir[i01]) + (1 - standY) * sinf(randDir[i01]);
			float p11 = (1 - standX) * cosf(randDir[i11]) + (1 - standY) * sinf(randDir[i11]);

			float u = fade(standX);
			float v = fade(standY);

			float n0 = lerp(p00, p01, v);
			float n1 = lerp(p10, p11, v);
			float result = lerp(n0, n1, u);

			float adjustHeight = (result + 1.0f) * 0.5f * 10 + 1;


			Vertex node;
			node.pos = { static_cast<float>(i), adjustHeight, static_cast<float>(j), 1.0f };
			XMVECTOR posVec = XMLoadFloat4(&node.pos);
			XMVECTOR norVec = XMVector3Normalize(posVec);
			XMStoreFloat4(&node.nor, norVec);

			mVertices.push_back(node);
		}
	}


	const int GRID_WIDTH = x * scale;
	const int GRID_HEIGHT = y * scale;

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