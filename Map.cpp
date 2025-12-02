#include"Map.h"

#include<cstdlib>
#include<iostream>
#include<fstream>

using namespace eastl;
using namespace DirectX;


#define ADD_PERLIN_LAYER(x, z, scale) \
    do { \
        auto newPerlin = Map::createVertex((x), (z), (scale)); \
        Map::updateVertexBuffer(newPerlin); \
    } while(0

const float PI = acosf(-1);

inline float lerp(float v0, float v1, float t)
{
	return v0 + t * (v1 - v0);
}

inline float fade(float t)
{
	return ((6 * t - 15) * t + 10) * t * t * t;
}



void Map::UpdateCameraPos(const XMFLOAT4& pos)
{
	mCamPos = pos;
}



void Map::Draw(void)
{
	ID3D11Buffer* cbWorld	 = mBase->GetCBObjectBuffer();   // World
	ID3D11Buffer* cbView	 = mBase->GetCBFrameBuffer();    // View
	ID3D11Buffer* cbProj	 = mBase->GetCBResizeBuffer(); // Projection
	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();


	CBObject cbObj;

	XMMATRIX scale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX translate = XMMatrixTranslation(-200.0f, -10.0f, -200.0f);

	cbObj.World = scale * translate;
	cbObj.World = XMMatrixTranspose(cbObj.World);


	ctx->UpdateSubresource(cbWorld, 0, nullptr, &cbObj, 0, 0);

	ctx->VSSetShader(mBase->GetVertexShader(eShaderID::Basic), nullptr, 0);
	ctx->PSSetShader(mBase->GetPixelShader(eShaderID::Basic), nullptr, 0);

	ctx->VSSetConstantBuffers(0, 1, &cbWorld); 
	ctx->VSSetConstantBuffers(1, 1, &cbView);  
	ctx->VSSetConstantBuffers(2, 1, &cbProj);  
	ctx->PSSetConstantBuffers(0, 1, &cbWorld);

	// IA 설정
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetInputLayout(mBase->GetInputLayout());
	ctx->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	updateChunkIndexBuffers();
}


bool Map::loadMeshData(void)
{
	uint32_t mainSeed = static_cast<uint32_t>(std::time(nullptr));

	/*std::ofstream fout(MAIN_SEED_FILE, std::ios::binary);
	if (!fout.is_open())
	{
		MessageBoxA(nullptr, "Cannot wirte seed", "Error", MB_OK);
		assert(false);
	}
	fout.write(reinterpret_cast<char*>(&mainSeed), sizeof(mainSeed));
	fout.close();*/
	
	std::ifstream fin(MAIN_SEED_FILE, std::ios::binary);
	if (!fin.is_open())
	{
		MessageBoxA(nullptr, "Cannot read seed", "Error", MB_OK);
		assert(false);
	}
	fin.read(reinterpret_cast<char*>(&mainSeed), sizeof(uint32_t));
	fin.close();

	srand(mainSeed);

	mVertices = createVertex(MAP_DIM, MAP_DIM, 1);
	createIndex(MAP_DIM, MAP_DIM);

	return true;
}


vector<Vertex> Map::createVertex(const int& x, const int& z, const int& scale)
{

	vector<float> randDir;
	vector<Vertex> vertices;

	const int COARSE_W = x * 2;
	const int COARSE_H = z * 2;
	randDir.reserve(COARSE_W * COARSE_H);

	for (int i = 0; i < COARSE_W * COARSE_H; ++i)
	{
		float angle = (rand() % 360) * PI / 180.0f;
		randDir.push_back(angle);
	}

	const int GRID_WIDTH  = x;
	const int GRID_HEIGHT = z;

	// store heights first
	eastl::vector<float> heights;
	heights.resize(GRID_WIDTH * GRID_HEIGHT);

	for (int j = 0; j < GRID_HEIGHT; ++j)
	{
		for (int i = 0; i < GRID_WIDTH; ++i)
		{
			const float px = ((float)i / (float)(GRID_WIDTH));
			const float py = ((float)j / (float)(GRID_HEIGHT));
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

			float adjustHeight = (result + 1.0f) * 0.5f * (10.0f * scale) + 1.0f;

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

			XMVECTOR n = XMVectorSet(-gx, 2.0f, -gz, 0.0f);
			n = XMVector3Normalize(n);
			XMFLOAT3 nf;
			XMStoreFloat3(&nf, n);
			node.Normal = { nf.x, nf.y, nf.z, 0.0f };

			vertices.push_back(node);
		}
	}

	return vertices;
}

void Map::createIndex(const int& x, const int& y)
{

	const int GRID_WIDTH  = x;
	const int GRID_HEIGHT = y;

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

	const int MAX_INDEX_BUFFER_SIZE = 120 * 120 * 6;

	ID3D11Device* dev = mBase->GetDevice();
	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));

	bufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * mVertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	initData.pSysMem = mVertices.data();

	hr = dev->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_VERTEX_BUFFER;
	} 

	{
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.ByteWidth = sizeof(uint32_t) * CHUNK_DIM * CHUNK_DIM * 6;

		for (int i = 0; i < INDEX_BUFFER_DIM; ++i)
		{
			for (int j = 0; j < INDEX_BUFFER_DIM; ++j)
			{
				vector<uint32_t> indices;
				const int pointX = j * CHUNK_DIM;
				const int pointZ = i * CHUNK_DIM;

				for (int z = pointZ; z < pointZ + CHUNK_DIM - 1; ++z)
				{
					for (int x = pointX; x < pointX + CHUNK_DIM - 1; ++x)
					{
						int p00 = x + MAP_DIM * z;
						int p01 = (x + 1) + MAP_DIM * z;
						int p10 = x + MAP_DIM * (z + 1);
						int p11 = (x + 1) + MAP_DIM * (z + 1);
						
						indices.push_back(p00);
						indices.push_back(p11);
						indices.push_back(p01);

						indices.push_back(p00);
						indices.push_back(p10);
						indices.push_back(p11);
					}
				}

				bufferDesc.ByteWidth = sizeof(uint32_t) * indices.size();
				initData.pSysMem = indices.data();

				ID3D11Buffer* indexBuffer = nullptr;
				hr = dev->CreateBuffer(&bufferDesc, &initData, &indexBuffer);
				if (FAILED(hr))
				{
					goto LB_FAILED_CREATE_INDEX_BUFFER;
				}
				mIndexBuffers.push_back(indexBuffer);
			}
		}
	}

	return true;

LB_FAILED_CREATE_INDEX_BUFFER:

	for (auto i = mIndexBuffers.begin(); i != mIndexBuffers.end(); ++i)
	{
		(*i)->Release();
	}
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

void Map::updateChunkIndexBuffers(void)
{
	const int EXTEND_DIM = 4;
	const int COORD_OFFSET = MAP_DIM / 2;

	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();


	int camX = static_cast<int>((mCamPos.x + COORD_OFFSET) / (float)EXTEND_DIM) / CHUNK_DIM;
	int camZ = static_cast<int>((mCamPos.z + COORD_OFFSET) / (float)EXTEND_DIM) / CHUNK_DIM;

	if (camX < 0 || camX >= INDEX_BUFFER_DIM || camZ < 0 || camZ >= INDEX_BUFFER_DIM)
		return;

	int minX = std::max(0, camX - 1);
	int maxX = std::min(INDEX_BUFFER_DIM - 1, camX + 1);
	int minZ = std::max(0, camZ - 1);
	int maxZ = std::min(INDEX_BUFFER_DIM - 1, camZ + 1);


	for (int z = minZ; z < maxZ; ++z)
	{
		for (int x = minX; x < maxX; ++x)
		{
			size_t indexCount = (CHUNK_DIM - 1) * (CHUNK_DIM - 1) * 6;

			ctx->IASetIndexBuffer(mIndexBuffers[x + INDEX_BUFFER_DIM * z], DXGI_FORMAT_R32_UINT, 0);
			ctx->DrawIndexed(static_cast<UINT>(indexCount), 0, 0);
		}
	}
}