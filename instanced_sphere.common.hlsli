#include "sphere.common.hlsli"

struct InstanceData
{
    float4x4 World;
    float2 TexCoord;
    float2 Padding;
    float Radius;
    float3 Velocity;
};