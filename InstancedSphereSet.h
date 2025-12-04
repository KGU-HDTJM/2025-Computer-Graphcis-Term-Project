#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <EASTL/vector.h>

#include "define.h"
#include "D3D11Base.h"
#include "SphereGenerator.h"


using namespace DirectX;
using namespace eastl;

class InstancedSphereSet
{
public:
	InstancedSphereSet(D3D11Base* base, SphereGenerator* generator, float radius, size_t count)
		: mBase(base), mGenerator(generator), InstCount(count)
	{
		mInstData = new vector<InstanceData>(count);
		mComputeData = new vector<ComputeBuf>(count);

		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;                // Dynamic for frequent updates
		vbDesc.ByteWidth = sizeof(InstanceData) * count;   // Size of buffer
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // Vertex buffer binding
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU write access
		vbDesc.MiscFlags = 0;

		HRESULT hr = mBase->GetDevice()->CreateBuffer(&vbDesc, nullptr, &mInstBuffer);
		
		InstanceData initInstData = {};
		XMStoreFloat4x4(&initInstData.World, XMMatrixIdentity());
		ComputeBuf initComBuf = {};
		initComBuf.Position = XMFLOAT4(0.F, -500.F, 0.F, 1.F);
		initComBuf.Radius = radius;
		initComBuf.Velocity = XMFLOAT4(0.F, 0.F, 0.F, 1.F);
		for (int i = 0; i < InstCount; ++i)
		{
			(*mInstData)[i] = initInstData;
			(*mComputeData)[i] = initComBuf;
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.ByteWidth = sizeof(ComputeBuf) * InstCount;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.StructureByteStride = sizeof(ComputeBuf);
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		// Initialize with data
		D3D11_SUBRESOURCE_DATA initSubresource = {};
		initSubresource.pSysMem = mComputeData->data();

		ID3D11Buffer* computeBuffer = nullptr;
		hr = mBase->GetDevice()->CreateBuffer(&bufferDesc, &initSubresource, &computeBuffer);

		Update(0.F);
	}
	~InstancedSphereSet(void);

	void Update(float deltaTime);
	void Draw(void);
	InstanceObject GetInstanceObject(int idx);

public:
	const size_t InstCount;
private:
	D3D11Base* mBase;
	const SphereGenerator* mGenerator;
	
	vector<InstanceData>* mInstData;
	vector<ComputeBuf>* mComputeData;
	ID3D11Buffer* mInstBuffer;
	ID3D11Buffer* mCSBuffer;
};

