#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "D3D11Base.h"
#include "SphereGenerator.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class Sphere
{
public:
	Sphere(D3D11Base* base, SphereGenerator* generator) 
		: mBase(base), mGenerator(generator)
		, mInputLayout(base->GetInputLayout())
		, mVertexBuffer(nullptr), mVertexCount(0)
	{
	}
	~Sphere();

	bool Initialize();
	void Draw();
	void Cleanup();

private:
	D3D11Base* mBase = nullptr;
	SphereGenerator* mGenerator = nullptr;

	ID3D11InputLayout* mInputLayout;
	ID3D11Buffer* mVertexBuffer;
	UINT mVertexCount;
};

