#pragma once

#include "D3D11Base.h"

class Sphere
{
public:
	Sphere(D3D11Base* base);
private:
	D3D11Base* mBase;
	
	static ID3D11VertexShader* msVertexShader;
	static ID3D11HullShader* msHullShader;
	static ID3D11DomainShader* msDomainShader;
};

