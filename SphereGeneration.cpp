#include "SphereGenerator.h"
#include "define.h"
#include "Sphere.h"

#define VERTEX_COUNT (8)
#define INDEX_COUNT (36)

static Vertex cubeVertices[VERTEX_COUNT] =
{
	{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT4(-1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT4(1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(1.0f, 1.0f) },


	{ XMFLOAT4(-1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT4(-1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT4(1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT4(1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.0f, 1.0f) },
};

// 큐브 인덱스 데이터
static const uint32_t CUBE_INDICES[INDEX_COUNT] =
{
	0,1,2, 0,2,3, // 앞
	4,6,5, 4,7,6, // 뒤
	4,5,1, 4,1,0, // 좌
	3,2,6, 3,6,7, // 우
	1,5,6, 1,6,2, // 상
	4,0,3, 4,3,7  // 하
};

SphereGenerator::SphereGenerator(D3D11Base* base)
{
	mBase = base;
	ID3DBlob* vsBlob = mBase->CompileShader((LPWSTR)L"sphere.vs.hlsl", "vs_5_0");
	ID3DBlob* hsBlob = mBase->CompileShader((LPWSTR)L"sphere.hs.hlsl", "hs_5_0");
	ID3DBlob* dsBlob = mBase->CompileShader((LPWSTR)L"sphere.ds.hlsl", "ds_5_0");
	ID3DBlob* psBlob = mBase->CompileShader((LPWSTR)L"sphere.ps.hlsl", "ps_5_0");

	mBase->GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader);
	mBase->GetDevice()->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &mHullShader);
	mBase->GetDevice()->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &mDomainShader);
	mBase->GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader);

	vsBlob->Release();
	hsBlob->Release();
	dsBlob->Release();
	psBlob->Release();

	for (int i = 0; i < 8; ++i)
	{
		cubeVertices[i].Normal = cubeVertices[i].Position;
		cubeVertices[i].Normal.w = 0.F;
	}
	initConstantBuffer();
}

SphereGenerator::~SphereGenerator()
{
	mVertexShader->Release();
	mHullShader->Release();
	mDomainShader->Release();
	mPixelShader->Release();
}

Sphere* SphereGenerator::CreateSphere(float radius, const XMFLOAT4& pos) const
{
	UINT vertexCount;
	ID3D11Buffer* vertexBuffer = createVertexBuffer(radius, vertexCount);
	UINT indexCount;
	ID3D11Buffer* indexBuffer = createIndexBuffer(indexCount);
	
	// get world matrix
	XMVECTOR posVec = XMLoadFloat4(&pos);
	XMMATRIX world = XMMatrixTranslationFromVector(posVec);

	Sphere* res = new Sphere(mBase, this, radius, vertexBuffer, indexBuffer, indexCount, world);
	return res;
}

ID3D11VertexShader* SphereGenerator::GetVertexShader(void) const
{
	return mVertexShader;
}

ID3D11HullShader* SphereGenerator::GetHullShader(void) const
{
	return mHullShader;
}

ID3D11DomainShader* SphereGenerator::GetDomainShader(void) const
{
	return mDomainShader;
}

ID3D11PixelShader* SphereGenerator::GetPixelShader(void) const
{
	return mPixelShader;
}

ID3D11Buffer* SphereGenerator::GetTessellationBuffer() const
{
	return mTessellationBuffer;
}

ID3D11Buffer* SphereGenerator::createVertexBuffer(float radius, UINT& count) const
{
	count = 8;
	Vertex* vertices = new Vertex[count];
	for (size_t i = 0; i < count; i++)
	{
		vertices[i] = cubeVertices[i];
		vertices[i].Position.x = vertices[i].Position.x * radius;
		vertices[i].Position.y = vertices[i].Position.y * radius;
		vertices[i].Position.z = vertices[i].Position.z * radius;
	}
	ID3D11Buffer* vertexBuffer = nullptr;

	// Create VertexBuffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));

	bufferDesc.ByteWidth = sizeof(Vertex) * count;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	initData.pSysMem = vertices;

	HRESULT hr = mBase->GetDevice()->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
	
	delete[] vertices;
	assert(!FAILED(hr));
	return vertexBuffer;
}

ID3D11Buffer* SphereGenerator::createIndexBuffer(UINT& count) const
{
	ID3D11Buffer* indexBuffer = nullptr;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));

	count = 36;
	bufferDesc.ByteWidth = sizeof(uint32_t) * count;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = CUBE_INDICES;

	HRESULT hr = mBase->GetDevice()->CreateBuffer(&bufferDesc, &initData, &indexBuffer);
	assert(!FAILED(hr));
	return indexBuffer;
}

void SphereGenerator::initConstantBuffer()
{
	mTessellationBuffer = nullptr;


	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(TessellationCB);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	HRESULT hr = mBase->GetDevice()->CreateBuffer(&cbd, nullptr, &mTessellationBuffer);
	assert(SUCCEEDED(hr));

	TessellationCB tessData;
	tessData.TessFactor = 8.0f; // 원하는 테셀레이션 팩터
	tessData.Padding = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	ID3D11DeviceContext* context = mBase->GetImmediateContext();
	context->UpdateSubresource(mTessellationBuffer, 0, nullptr, &tessData, 0, 0);
}




