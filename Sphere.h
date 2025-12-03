#pragma once

#include <DirectXMath.h>

#include "define.h"
#include "D3D11Base.h"
#include "SphereGenerator.h"

using namespace DirectX;

class Sphere
{
public:
	Sphere(D3D11Base* base, const SphereGenerator* generator, float radius, const XMFLOAT4& position) 
		: mBase(base), mGenerator(generator), mRadius(radius), mPosition(position)
	{
		XMMATRIX scale = XMMatrixScaling(mRadius, mRadius, mRadius);
		XMMATRIX trans = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		XMStoreFloat4x4(&mCBObject.World, XMMatrixTranspose(scale * trans));
		
	}
	~Sphere();

	void Draw(void);
	void SetPosition(const XMFLOAT4& position);

private:
	D3D11Base* mBase;
	const SphereGenerator* mGenerator;
	CBObject mCBObject;
	float mRadius;
	XMFLOAT4 mPosition;
};

