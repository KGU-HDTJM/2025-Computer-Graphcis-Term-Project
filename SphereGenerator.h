#pragma once

#include "D3D11Base.h"
#include "define.h"

class Sphere;
class InstancedSphere;
class SphereGenerator
{

public:
	SphereGenerator(D3D11Base* base);
	~SphereGenerator();
	Sphere* CreateSphere(float radius, const XMFLOAT4& pos) const;
	InstancedSphere* CreateInstancedSphere(UINT count);
	
	ID3D11VertexShader* GetVertexShader(void) const;
	ID3D11HullShader* GetHullShader(void) const ;
	ID3D11DomainShader* GetDomainShader(void) const;
	ID3D11PixelShader* GetPixelShader(void) const;
	ID3D11Buffer* GetTessellationBuffer() const;
	ID3D11Buffer* GetVertexBuffer(void) const;
	ID3D11Buffer* GetIndexBuffer(UINT& count) const;

private:
	ID3D11Buffer* createVertexBuffer(UINT& count) const;
	ID3D11Buffer* createIndexBuffer(UINT& count) const;
	void initConstantBuffer();
	vector<InstanceData>* createInstanceData(UINT count) const;
	
private:
	D3D11Base* mBase;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	UINT mIndexCount;
	ID3D11VertexShader* mVertexShader;
	ID3D11HullShader* mHullShader;
	ID3D11DomainShader* mDomainShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11Buffer* mTessellationBuffer;
};

