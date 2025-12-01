#include "Sphere.h"

#include <d3d11.h>
#include "define.h"

using namespace DirectX;

Sphere::~Sphere()
{	
}

void Sphere::Draw(void)
{
	ID3D11Buffer* cbWorld = mBase->GetCBObjectBuffer();   // World
	ID3D11Buffer* cbView = mBase->GetCBFrameBuffer();    // View
	ID3D11Buffer* cbProj = mBase->GetCBResizeBuffer(); // Projection
	ID3D11DeviceContext* ctx = mBase->GetImmediateContext();
	
	ID3D11Buffer* vertexBuffer = mGenerator->GetVertexBuffer();
	UINT indexCount;
	ID3D11Buffer* indexBuffer = mGenerator->GetIndexBuffer(indexCount);

	// Prepare per-frame CB (World)
	CBObject cbObject;
	cbObject.World = XMMatrixTranspose(mCBObject.World);

	ctx->UpdateSubresource(cbWorld, 0, nullptr, &cbObject, 0, 0);


	// Set shaders
	ctx->VSSetShader(mGenerator->GetVertexShader(), nullptr, 0);

	// IMPORTANT: shader registers in sphere.common.hlsli are:
	//   b0 -> World, b1 -> View, b2 -> Projection, b3 -> TessellationCB
	// Bind buffers to match those registers for each shader stage that uses them.
	// Bind to VS
	ctx->VSSetConstantBuffers(0, 1, &cbWorld);             // b0 = World
	ctx->VSSetConstantBuffers(1, 1, &cbView);    // b1 = View
	ctx->VSSetConstantBuffers(2, 1, &cbProj); // b2 = Projection

	// Hull shader: bind HS and its tessellation CB (b3)
	ctx->HSSetShader(mGenerator->GetHullShader(), nullptr, 0);
	ID3D11Buffer* tessFactor = mGenerator->GetTessellationBuffer();
	ctx->HSSetConstantBuffers(3, 1, &tessFactor);

	// Domain shader: bind DS and also provide the World/View/Projection CBs to DS stage
	ctx->DSSetShader(mGenerator->GetDomainShader(), nullptr, 0);
	ctx->DSSetConstantBuffers(0, 1, &cbWorld);             // b0 = World
	ctx->DSSetConstantBuffers(1, 1, &cbView);    // b1 = View
	ctx->DSSetConstantBuffers(2, 1, &cbProj); // b2 = Projection

	// Pixel shader
	ctx->PSSetShader(mGenerator->GetPixelShader(), nullptr, 0);
	ctx->PSSetConstantBuffers(0, 1, &cbWorld);

	// IA setup
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Use 3-control-point patches for tessellation
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	// Draw
	ctx->DrawIndexed(indexCount, 0, 0);
	ctx->HSSetShader(nullptr, nullptr, 0);
	ctx->DSSetShader(nullptr, nullptr, 0);
}