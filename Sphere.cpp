#include "Sphere.h"


constexpr int RADIUS = 5;
constexpr int ANGLE_SEGMENT = 10;

using namespace eastl;
using namespace DirectX;


void Sphere::CleanUp(void)
{
	if (mIndexBuffer) { mIndexBuffer->Release(); }
	if (mVertexBuffer) { mVertexBuffer->Release(); }
}

ID3D11Buffer* Sphere::GetIndexBuffer(void) const
{
	return mIndexBuffer;
}

ID3D11Buffer* Sphere::GetVertexBuffer(void) const {
	return mVertexBuffer;
}

UINT Sphere::GetIndexCount(void) const {
	return mIndexCount;
}

void Sphere::setupSphere(void)
{
	const float ANGLE_STEP = XM_PI / ANGLE_SEGMENT;

	for (int i = 0; i <= ANGLE_SEGMENT; ++i)
	{
		const float latitude = XM_PI - ANGLE_STEP * i;

		for (int j = 0; j < ANGLE_SEGMENT * 2; ++j)
		{
			const float longtitude = ANGLE_STEP * j;

			Vertex pos;

			pos.position.x = cosf(latitude) * RADIUS * cosf(longtitude);
			pos.position.y = cosf(latitude) * RADIUS * sinf(longtitude);
			pos.position.w = 1;
			pos.position.z = sinf(latitude) * RADIUS;

			XMVECTOR normalVec = XMVector3Normalize(XMLoadFloat4(&pos.position));
			XMStoreFloat4(&pos.normal, normalVec);

			pos.texcoord.x = 0;
			pos.texcoord.y = 0;

			mVertices.emplace_back(pos);
		}
	}

	
	const int COLUMNS = ANGLE_SEGMENT * 2;

	for (int i = 0; i < COLUMNS; ++i)
	{
	

		for (int j = 0; j < COLUMNS; ++j)
		{
			uint32_t i00 = i * COLUMNS + j;
			uint32_t i10 = i * COLUMNS + (j + 1) % COLUMNS;
			uint32_t i01 = (i + 1) * COLUMNS + j;
			uint32_t i11 = (i + 1) * COLUMNS + (j + 1) % COLUMNS;

			mIndices.push_back(i00);
			mIndices.push_back(i10);
			mIndices.push_back(i01);

			mIndices.push_back(i10);
			mIndices.push_back(i01);
			mIndices.push_back(i11);
		}
	}

	mIndexCount = mIndices.size();
}


bool Sphere::createMeshBuffers(void)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * mVertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));

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