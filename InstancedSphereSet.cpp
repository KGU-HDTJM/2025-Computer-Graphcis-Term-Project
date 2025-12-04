#include "InstancedSphereSet.h"

void InstancedSphereSet::Update(float deltaTime)
{
	// TODO: get compute shader result
	for (size_t i = 0; i < InstCount; ++i)
	{
		ComputeBuf computeData = mComputeData->at(i);
		// Update position using velocity
		XMVECTOR pos = XMLoadFloat4(&computeData.Position);
		XMVECTOR vel = XMLoadFloat4(&computeData.Velocity);
		XMVECTOR dVel = vel * deltaTime;

		// CPU collision detection
		if (bCollisionResponse)
		{
			XMVECTOR newVel = vel;
			for (size_t j = 0; j < InstCount; ++j)
			{
				if (i == j)
				{
					continue;
				}
				ComputeBuf temp = mComputeData->at(j);
				XMVECTOR diff = XMLoadFloat4(&temp.Position) - pos;
				float radiusSum = temp.Radius + computeData.Radius;
				if (XMVector3Length(diff).m128_f32[0] < radiusSum)
				{
					XMVECTOR cv = XMLoadFloat4(&temp.Velocity);

					XMVECTOR adder = diff * XMVector3Dot(XMVector3Normalize(newVel), XMVector3Normalize(cv));
					newVel += adder;
				}
			}
			XMStoreFloat4(&computeData.Velocity, newVel);
			computeData.Velocity.w = 0.F;
		}
		//================CPU Collision detectoin===========
		if (XMVector3Length(pos + dVel).m128_f32[0] < SPHERE_VOLUME)
		{
			XMStoreFloat4(&computeData.Position, pos + dVel);
		}
		else
		{
			XMStoreFloat4(&computeData.Position, pos);
			XMStoreFloat4(&computeData.Velocity, -vel);
		}

		// Optional: apply bounds or wrap-around logic
		if (computeData.Position.y < -500.0f)
		{
			computeData.Position.y = -500.0f;
			computeData.Velocity = XMFLOAT4(0.F, 0.F, 0.F, 0.F);
		}

		(*mSwapData)[i] = computeData;

		// Build world matrix for rendering
		XMMATRIX scale = XMMatrixScaling(computeData.Radius, computeData.Radius, computeData.Radius);
		XMMATRIX translate = XMMatrixTranslation(computeData.Position.x, computeData.Position.y, computeData.Position.z);
		XMMATRIX world = scale * translate;

		XMStoreFloat4x4(&mInstData->at(i).World, world);
	}
	// TODO: update InstBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Map the buffer for writing
	HRESULT hr = mBase->GetImmediateContext()->Map(mInstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		// Copy new instance data into the buffer
		memcpy(mappedResource.pData, mInstData->data(), sizeof(InstanceData) * mInstData->size());

		// Unmap after writing
		mBase->GetImmediateContext()->Unmap(mInstBuffer, 0);
	}
	vector<ComputeBuf>* temp = mSwapData;
	mSwapData = mComputeData;
	mComputeData = temp;
}

InstancedSphereSet::~InstancedSphereSet(void)
{
	mInstBuffer->Release();
	// mCSBuffer->Release();

	delete mInstData;
	delete mComputeData;
}

void InstancedSphereSet::Draw(void)
{
	ID3D11DeviceContext* context = mBase->GetImmediateContext();
	
	ID3D11Buffer* cbObj = mBase->GetCBObjectBuffer();
	ID3D11Buffer* cbFrame = mBase->GetCBFrameBuffer();
	ID3D11Buffer* cbResize = mBase->GetCBResizeBuffer();

	UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* buffers[2] = { mGenerator->GetVertexBuffer(), mInstBuffer};
	UINT indexCount;
	context->IASetInputLayout(mGenerator->GetInstInputLayout());
	context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	context->IASetIndexBuffer(mGenerator->GetIndexBuffer(indexCount), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set shaders
	context->VSSetShader(mGenerator->GetInstVertexShader(), nullptr, 0);
	//context->VSSetConstantBuffers(0, 1, &cbObj);
	context->VSSetConstantBuffers(1, 1, &cbFrame);
	context->VSSetConstantBuffers(2, 1, &cbResize);

	context->HSSetShader(mGenerator->GetInstHullShader(), nullptr, 0);
	
	ID3D11Buffer* tessFactor = mGenerator->GetTessellationBuffer();
	context->HSSetConstantBuffers(3, 1, &tessFactor);

	context->DSSetShader(mGenerator->GetInstDomainShader(), nullptr, 0);
	//context->DSSetConstantBuffers(0, 1, &cbObj);
	context->DSSetConstantBuffers(1, 1, &cbFrame);
	context->DSSetConstantBuffers(2, 1, &cbResize);

	context->PSSetShader(mGenerator->GetInstPixelShader(), nullptr, 0);
	//context->PSSetConstantBuffers(0, 1, &cbObj);
	context->PSSetConstantBuffers(1, 1, &cbFrame);
	context->PSSetConstantBuffers(2, 1, &cbResize);
	
	// TODO: Run compute shader

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	// Draw instanced
	context->DrawIndexedInstanced(indexCount, InstCount, 0, 0, 0);
	context->HSSetShader(nullptr, nullptr, 0);
	context->DSSetShader(nullptr, nullptr, 0);
}

InstanceObject InstancedSphereSet::GetInstanceObject(int idx)
{
	// TODO: insert return statement here
	InstanceObject res;

	res.ComputeData = mComputeData->data() + idx;
	res.InstData = mInstData->data() + idx;

	return res;
}
