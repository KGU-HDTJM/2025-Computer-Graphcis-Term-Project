#include"Map.h"

#include<cstdlib>
#include<iostream>
#include<fstream>

using namespace eastl;
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

size_t Map::GetIndexCount(void)
{
	return mIndices.size();
}

ID3D11Buffer* Map::GetIndexBuffer(void)
{
	return mIndexBuffer;
}

ID3D11Buffer* Map::GetVertexBuffer(void)
{
	return mVertexBuffer;
}

size_t Map::GetVertexSize(void) const 
{
	return sizeof(Vertex);
}


void Map::AddPerlinLayer(int x, int y, int scale)
{
	vector<Vertex> newPerlin = createVertex(x, y, scale);
	updateVertexBuffer(newPerlin);
}

void Map::Draw(void)
{
	ID3D11Buffer* cbWorld	 = mBase->GetCBObjectBuffer();   // World
	ID3D11Buffer* cbView	 = mBase->GetCBFrameBuffer();    // View
	ID3D11Buffer* cbProj	 = mBase->GetCBResizeBuffer(); // Projection
	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();

	CBObject cbObj;

	XMMATRIX scale = XMMatrixScaling(3.0f, 1.0f, 3.0f);


	XMMATRIX translate = XMMatrixTranslation(-40.0f, -10.0f, -40.0f);
	cbObj.World = scale * translate;
	cbObj.World = XMMatrixTranspose(cbObj.World);


	ctx->UpdateSubresource(cbWorld, 0, nullptr, &cbObj, 0, 0);

	ctx->VSSetShader(mBase->GetVertexShader(eShaderID::Basic), nullptr, 0);
	ctx->PSSetShader(mBase->GetPixelShader(eShaderID::Basic), nullptr, 0);

	ctx->VSSetConstantBuffers(0, 1, &cbWorld); 
	ctx->VSSetConstantBuffers(1, 1, &cbView);  
	ctx->VSSetConstantBuffers(2, 1, &cbProj);  
	ctx->HSSetShader(nullptr, nullptr, 0);
	ctx->DSSetShader(nullptr, nullptr, 0);
	ctx->PSSetConstantBuffers(0, 1, &cbWorld);

	// IA 설정
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetInputLayout(mBase->GetInputLayout());
	ctx->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	ctx->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	ctx->DrawIndexed(static_cast<UINT>(mIndices.size()), 0, 0);
}


vector<Vertex> Map::createVertex(const int& x, const int& y, const int& scale)
{
	vector<float> randDir;
	vector<Vertex> vertices;

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

	for (int j = 0; j < GRID_HEIGHT; ++j)
	{
		for (int i = 0; i < GRID_WIDTH; ++i)
		{
			Vertex node;
			float h = heights[i + GRID_WIDTH * j];
			node.Position = { static_cast<float>(i), h, static_cast<float>(j), 1.0f };

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
			node.Normal = { nf.x, nf.y, nf.z, 0.0f };

			vertices.push_back(node);
		}
	}

	// - store vector to binary -
	std::ofstream fout(VERTEX_FILE, std::ios::binary);
	if (!fout.is_open())
	{
		MessageBoxA(nullptr, "VERTEX_FILE not fount", "Error", MB_OK);
		assert(false);
	}
	fout.write(reinterpret_cast<char*>(vertices.data()), vertices.size() * sizeof(Vertex));
	fout.close();

	return vertices;
}

void Map::createIndex(const int& x, const int& y, const int& scale)
{

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

bool Map::createBuffers(void)
{
	HRESULT hr;

	std::ifstream fin(VERTEX_FILE, std::ios::binary);
	if (!fin.is_open())
	{
		MessageBoxA(nullptr, "VERTEX_FILE not fount", "Error", MB_OK);
		assert(false);
	}
	fin.seekg(0, std::ios::end);
	std::streamsize size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	size_t vecSize = size / sizeof(Vertex);
	
	vector<Vertex> vertices(vecSize);

	fin.read(reinterpret_cast<char*>(vertices.data()), vecSize * sizeof(Vertex));
	fin.close();

	ID3D11Device* dev = mBase->GetDevice();
	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));

	bufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	initData.pSysMem = vertices.data();

	hr = dev->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_VERTEX_BUFFER;
	}

	bufferDesc.ByteWidth = (UINT)(sizeof(uint32_t) * mIndices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = mIndices.data();

	hr = dev->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
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

void Map::updateVertexBuffer(const vector<Vertex>& newPerlin)
{
	const size_t BASE_LENGTH = mVertices.size();
	const size_t HALF_BASE_LENGTH = BASE_LENGTH / 2;
	const size_t LAYER_LENGTH = newPerlin.size();


	size_t standIdx = rand() % HALF_BASE_LENGTH;

	for (size_t i = 0; i < LAYER_LENGTH && standIdx + i < BASE_LENGTH; ++i)
	{
		XMFLOAT4 result;
		XMVECTOR layerPos = XMLoadFloat4(&newPerlin[i].Position);
		XMVECTOR originPos = XMLoadFloat4(&mVertices[standIdx + i].Position);
		
		layerPos = XMVectorAdd(originPos, layerPos);
		XMStoreFloat4(&result, layerPos);
		result.w = 1.0f;

		mVertices[standIdx + i].Position = result;
	}

	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();

	ctx->UpdateSubresource(mVertexBuffer, 0, nullptr, mVertices.data(), 0, 0);
}