#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT4 Position;
	XMFLOAT4 Normal;
	XMFLOAT2 TexCoord;
};

struct CBFrame
{
	XMMATRIX View;
};

struct CBResize
{
	XMMATRIX Projection;
};

struct CBObject {
	XMMATRIX World;
};

struct TessellationCB
{
	float TessFactor;
	DirectX::XMFLOAT3 Padding;
};
