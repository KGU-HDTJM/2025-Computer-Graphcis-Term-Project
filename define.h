#pragma once
#include <DirectXMath.h>

using namespace DirectX;

#define FAR_DISTANCE (1000.F)
#define INSTANCE_COUNT (100)
#define SPHERE_VOLUME (1000)

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
	XMFLOAT3 Padding;
};

struct InstanceObject
{
	InstanceData* InstData;
	ComputeBuf* ComputeData;
};

struct CBCompute
{
	uint32_t FrameOffset; // Rotating offset
	uint32_t InstanceCount; // Total objects
	float DeltaTime;
	float Padding;
};
