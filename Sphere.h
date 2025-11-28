#pragma once

#include "D3D11Base.h"

class Sphere
{
public:
	Sphere(D3D11Base* base);
public:

	ID3D11VertexShader* sVertexShader;
	ID3D11HullShader* sHullShader;
	ID3D11DomainShader* sDomainShader;
	ID3D11PixelShader* sPixelShader;
private:
	D3D11Base* mBase;
	
};

