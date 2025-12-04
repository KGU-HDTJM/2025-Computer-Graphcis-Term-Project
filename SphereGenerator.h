#pragma once

#include "D3D11Base.h"


class Sphere;
class InstancedSphereSet;
class SphereGenerator
{

public:
	SphereGenerator(D3D11Base* base);
	~SphereGenerator();
	Sphere* CreateSphere(float radius, const XMFLOAT4& pos) const;
	InstancedSphereSet* CreateSphereSet(float radius, const XMFLOAT4& pos, size_t count);

	// common
	ID3D11Buffer* GetTessellationBuffer() const;
	ID3D11Buffer* GetVertexBuffer(void) const;
	ID3D11Buffer* GetIndexBuffer(UINT& count) const;
	// for sphere
	ID3D11VertexShader* GetVertexShader(void) const;
	ID3D11HullShader* GetHullShader(void) const ;
	ID3D11DomainShader* GetDomainShader(void) const;
	ID3D11PixelShader* GetPixelShader(void) const;
	// for sphere set
	ID3D11VertexShader* GetInstVertexShader(void) const;
	ID3D11HullShader* GetInstHullShader(void) const;
	ID3D11DomainShader* GetInstDomainShader(void) const;
	ID3D11PixelShader* GetInstPixelShader(void) const;
	ID3D11ComputeShader* GetInstComputeShader(void) const;
	ID3D11InputLayout* GetInstInputLayout(void) const;
	ID3D11Buffer* GetCBTimeBuffer(void) const;

private:
	ID3D11Buffer* createVertexBuffer(UINT& count) const;
	ID3D11Buffer* createIndexBuffer(UINT& count) const;
	void initConstantBuffer();

private:
	// common
	D3D11Base* mBase;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	UINT mIndexCount;
	ID3D11Buffer* mTessellationBuffer;
	// for sphere
	ID3D11VertexShader* mVertexShader;
	ID3D11HullShader* mHullShader;
	ID3D11DomainShader* mDomainShader;
	ID3D11PixelShader* mPixelShader;
	// for sphere set
	ID3D11VertexShader* mInstVertexShader;
	ID3D11HullShader* mInstHullShader;
	ID3D11DomainShader* mInstDomainShader;
	ID3D11PixelShader* mInstPixelShader;
	ID3D11ComputeShader* mInstComputeShader;
	ID3D11InputLayout* mInstInputLayout;
	ID3D11Buffer* mCBCompute;
};

