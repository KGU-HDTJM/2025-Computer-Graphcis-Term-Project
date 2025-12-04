#include "SphereGenerator.h"
#include "define.h"
#include "Sphere.h"
#include "InstancedSphereSet.h"

#define VERTEX_COUNT (8)
#define INDEX_COUNT (36)

Vertex cubeVertices[VERTEX_COUNT] =
{
	{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
	{ XMFLOAT4(-1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
	{ XMFLOAT4(1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
	{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },

	{ XMFLOAT4(-1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
	{ XMFLOAT4(-1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
	{ XMFLOAT4(1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
	{ XMFLOAT4(1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMFLOAT2(0.333F, 1.0F) },
};

// 큐브 인덱스 데이터
const uint32_t CUBE_INDICES[INDEX_COUNT] =
{
	0,1,2, 0,2,3,
	4,6,5, 4,7,6,
	4,5,1, 4,1,0,
	3,2,6, 3,6,7,
	1,5,6, 1,6,2,
	4,0,3, 4,3,7
};

#define INST_INPUT_LAYOUT_ELEMENT (7)
D3D11_INPUT_ELEMENT_DESC InstanceInputLayout[INST_INPUT_LAYOUT_ELEMENT] =
{
	// Per-vertex data (slot 0)
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	// Per-instance data (slot 1)s
	{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};


SphereGenerator::SphereGenerator(D3D11Base* base)
{
	// shpere shader
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
	
	// sphere set shader
	// create inputlayout ( compile shaders -> create inputlayout)
	vsBlob = mBase->CompileShader((LPWSTR)L"instanced_sphere.vs.hlsl", "vs_5_0");
	hsBlob = mBase->CompileShader((LPWSTR)L"instanced_sphere.hs.hlsl", "hs_5_0");
	dsBlob = mBase->CompileShader((LPWSTR)L"instanced_sphere.ds.hlsl", "ds_5_0");
	psBlob = mBase->CompileShader((LPWSTR)L"instanced_sphere.ps.hlsl", "ps_5_0");
	ID3DBlob* csBlob = mBase->CompileShader((LPWSTR)L"instanced_sphere.cs.hlsl", "cs_5_0");

	// create input layout
	mBase->GetDevice()->CreateInputLayout(InstanceInputLayout, INST_INPUT_LAYOUT_ELEMENT, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInstInputLayout);

	mBase->GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mInstVertexShader);
	mBase->GetDevice()->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &mInstHullShader);
	mBase->GetDevice()->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &mInstDomainShader);
	mBase->GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mInstPixelShader);
	mBase->GetDevice()->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &mInstComputeShader);

	vsBlob->Release();
	hsBlob->Release();
	dsBlob->Release();
	psBlob->Release();
	csBlob->Release();

	for (int i = 0; i < 8; ++i)
	{
		cubeVertices[i].Normal = cubeVertices[i].Position;
		cubeVertices[i].Normal.w = 0.F;
	}
	initConstantBuffer();
	UINT count;
	mVertexBuffer = createVertexBuffer(count);
	mIndexBuffer = createIndexBuffer(count);
	mIndexCount = count;
}

SphereGenerator::~SphereGenerator()
{
	mVertexShader->Release();
	mHullShader->Release();
	mDomainShader->Release();
	mPixelShader->Release();
	mTessellationBuffer->Release();
	mVertexBuffer->Release();
	mIndexBuffer->Release();

	mInstVertexShader->Release();
	mInstHullShader->Release();
	mInstDomainShader->Release();
	mInstPixelShader->Release();
	mInstComputeShader->Release();
	mInstInputLayout->Release();
	mCBCompute->Release();

}

Sphere* SphereGenerator::CreateSphere(float radius, const XMFLOAT4& pos) const
{
	Sphere* res = new Sphere(mBase, this, radius, pos);
	return res;	
}

InstancedSphereSet* SphereGenerator::CreateSphereSet(float radius, const XMFLOAT4& pos, size_t count)
{
	// TODO: call SphereSet constructor
	InstancedSphereSet* res = new InstancedSphereSet(mBase, this, radius, count);
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

ID3D11VertexShader* SphereGenerator::GetInstVertexShader(void) const
{
	return mInstVertexShader;
}

ID3D11HullShader* SphereGenerator::GetInstHullShader(void) const
{
	return mInstHullShader;
}

ID3D11DomainShader* SphereGenerator::GetInstDomainShader(void) const
{
	return mInstDomainShader;
}

ID3D11PixelShader* SphereGenerator::GetInstPixelShader(void) const
{
	return mInstPixelShader;
}

ID3D11ComputeShader* SphereGenerator::GetInstComputeShader(void) const
{
	return mInstComputeShader;
}

ID3D11InputLayout* SphereGenerator::GetInstInputLayout(void) const
{
	return mInstInputLayout;
}

ID3D11Buffer* SphereGenerator::GetCBTimeBuffer(void) const
{
	return mCBCompute;
}

ID3D11Buffer* SphereGenerator::GetTessellationBuffer() const
{
	return mTessellationBuffer;
}

ID3D11Buffer* SphereGenerator::GetVertexBuffer(void) const
{
	return mVertexBuffer;
}

ID3D11Buffer* SphereGenerator::GetIndexBuffer(UINT& count) const
{
	count = mIndexCount;
	return mIndexBuffer;
}

ID3D11Buffer* SphereGenerator::createVertexBuffer(UINT& count) const
{
	count = 8;
	Vertex* vertices = new Vertex[count];
	for (size_t i = 0; i < count; i++)
	{
		vertices[i] = cubeVertices[i];
		vertices[i].Position.x = vertices[i].Position.x;
		vertices[i].Position.y = vertices[i].Position.y;
		vertices[i].Position.z = vertices[i].Position.z;
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
	tessData.TessFactor = 8.0f;
	tessData.Padding = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	ID3D11DeviceContext* context = mBase->GetImmediateContext();
	context->UpdateSubresource(mTessellationBuffer, 0, nullptr, &tessData, 0, 0);

	mCBCompute = nullptr;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBCompute);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = mBase->GetDevice()->CreateBuffer(&cbd, nullptr, &mCBCompute);
	assert(SUCCEEDED(hr));

	CBCompute cbCompute = {};
	cbCompute.DeltaTime = 0.F;
	cbCompute.FrameOffset = 0.F;
	cbCompute.InstanceCount = 0.F;
	context->UpdateSubresource(mCBCompute, 0, nullptr, &cbCompute, 0, 0);
}




