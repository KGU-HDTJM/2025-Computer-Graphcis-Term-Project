#include "default.common.hlsli"

cbuffer TessellationCB : register(b3)
{
    float TessFactor;
    float3 Padding;
};

struct InstanceData
{
    float4x4 World;
    float2 TexCoord;
    float2 Padding;
    float Radius;
    float3 Velocity;
};