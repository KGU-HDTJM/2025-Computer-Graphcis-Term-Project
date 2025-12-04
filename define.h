#pragma once
#include <DirectXMath.h>

using namespace DirectX;

#define FAR_DISTANCE (100000.F)

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
	XMFLOAT4X4 World;
};

struct ComputeBuf
{
	XMFLOAT4 Velocity;
	XMFLOAT4 Position;
	float Radius;
	XMFLOAT4 Padding;
};

struct InstanceObject
{
	InstanceData* InstData;
	ComputeBuf* ComputeData;
};

struct CBTime
{
	float DeltaTime;
	XMFLOAT3 TimeCBPadding;
};
