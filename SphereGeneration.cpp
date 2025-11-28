#include "SphereGenerator.h"

SphereGenerator::SphereGenerator(D3D11Base* base)
{
	mBase = base;
	ID3DBlob* vsBlob = mBase->CompileShader((LPWSTR)L"sphere.vs.hlsl", "vs_5_0");
	ID3DBlob* hsBlob = mBase->CompileShader((LPWSTR)L"sphere.hs.hlsl", "hs_5_0");
	ID3DBlob* dsBlob = mBase->CompileShader((LPWSTR)L"sphere.ds.hlsl", "ds_5_0");
	ID3DBlob* psBlob = mBase->CompileShader((LPWSTR)L"sphere.ps.hlsl", "ps_5_0");

	mBase->GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &sVertexShader);
	mBase->GetDevice()->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &sHullShader);
	mBase->GetDevice()->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &sDomainShader);
	mBase->GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &sPixelShader);

	vsBlob->Release();
	hsBlob->Release();
	dsBlob->Release();
	psBlob->Release();
}




