#pragma once

#include "D3D11Base.h"

class Sphere
{
public:
	Sphere(D3D11Base* base);
private:
	D3D11Base* mBase;
	
	static ID3D11VertexShader* mVertexShader;
	static ID3D11HullShader* mHullShader;
	static ID3D11DomainShader* mDomainShader;
};

