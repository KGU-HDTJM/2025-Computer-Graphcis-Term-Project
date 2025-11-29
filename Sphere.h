#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "define.h"
#include "D3D11Base.h"
#include "SphereGenerator.h"

using namespace DirectX;

class Sphere
{
public:
	Sphere(D3D11Base* base, const SphereGenerator* generator, float radius, 
		ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT indexCount, const XMMATRIX& world) 
		: mBase(base), mGenerator(generator), mVertexBuffer(vertexBuffer), mIndexBuffer(indexBuffer), mIndexCount(indexCount), mRadius(radius)
	{
		mCBFrame.World = world;
	}
	~Sphere();

	void Draw(void);


private:
	D3D11Base* mBase;
	const SphereGenerator* mGenerator;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	UINT mIndexCount;
	float mRadius;
	CBFrame mCBFrame;
};

