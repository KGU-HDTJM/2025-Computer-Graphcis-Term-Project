#include"Perlin.h"

#include<ctime>
#include<cstdlib>

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


void Perlin::createNoise(int x, int y, int scale)
{
	const float standX = (rand() % 10) / 10.0f;
	const float standY = (rand() % 10) / 10.0f;

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
			int i00 = i * 2 +	  SCALED_X * j * 2;
			int i01 = i * 2 +	  SCALED_X * (j * 2+ 1);
			int i10 = i * 2 + 1 + SCALED_X * j * 2;
			int i11 = i * 2 + 1 + SCALED_X * (j * 2 + 1);

			float p00 = (0 - standX) * cosf(randDir[i00]) + (0 - standY) * sinf(randDir[i00]);
			float p10 = (1 - standX) * cosf(randDir[i10]) + (0 - standY) * sinf(randDir[i10]);
			float p01 = (0 - standX) * cosf(randDir[i01]) + (1 - standY) * sinf(randDir[i01]);
			float p11 = (1 - standX) * cosf(randDir[i11]) + (1 - standY) * sinf(randDir[i11]);

			float u = lerp(p00, p01, standY);
			float v = lerp(p10, p11, standY);
			float result = lerp(u, v, standX);

			Vertex node;
			node.pos = { static_cast<float>(x), static_cast<float>(y), result, 1 };
			XMVECTOR posVec = XMLoadFloat4(&node.pos);
			XMVECTOR norVec = XMVector3Normalize(posVec);
			XMStoreFloat4(&node.nor, norVec);

			mVertices.push_back(node);
		}
	}


	for (int j = 0; j < y * scale; j += 2)
	{
		for (int i = 0; i < x * scale; i += 2)
		{
			int p00 = i + x * scale * j;
			int p01 = i + 1 + x * scale * j;
			int p11 = i + 1 + x * scale * (j + 1);
			mIndices.push_back(p00);
			mIndices.push_back(p01);
			mIndices.push_back(p11);

			int p10 = i + x * scale * (j + 1);
			mIndices.push_back(p00);
			mIndices.push_back(p10);
			mIndices.push_back(p11);
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

	bufferDesc.ByteWidth = sizeof(int) * mIndices.size();
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