#pragma once
#include <DirectXMath.h>

using namespace DirectX;

#define FAR_DISTANCE (1000.F)

struct Vertex
{
	XMFLOAT4 Position;
	XMFLOAT4 Normal;
	XMFLOAT2 TexCoord;
};

struct CBFrame
{
	XMFLOAT4X4 View;
	XMFLOAT4 LightPos;
	XMFLOAT4 LightCL; // Light Color(xyz) and Lumen(w);
};

struct CBResize
{
	XMFLOAT4X4 Projection;
};

struct CBObject {
	XMFLOAT4X4 World;
};

struct TessellationCB
{
	float TessFactor;
	DirectX::XMFLOAT3 Padding;
};

struct InstanceData
{
	CBObject BaseCB;
	
	XMFLOAT2 TexCoord;
	XMFLOAT2 _Padding;
	float Radius;
	XMFLOAT3 Velocity;
};
