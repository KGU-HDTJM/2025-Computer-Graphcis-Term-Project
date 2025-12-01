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
	Sphere(D3D11Base* base, const SphereGenerator* generator, float radius, const XMFLOAT4& position) 
		: mBase(base), mGenerator(generator), mRadius(radius), mPosition(position)
	{
		XMMATRIX scale = XMMatrixScaling(radius, radius, radius);
		XMMATRIX trans = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		mCBObject.World = XMMatrixMultiply(scale, trans);
	}
	~Sphere();

	void Draw(void);


private:
	D3D11Base* mBase;
	const SphereGenerator* mGenerator;
	CBObject mCBObject;
	float mRadius;
	XMFLOAT4 mPosition;
};

