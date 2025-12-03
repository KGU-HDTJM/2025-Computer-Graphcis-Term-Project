#include "sphere.common.hlsli"
cbuffer TimeCB : register(b4)
{
    float DeltaTime;
    float3 TimeCBPadding;
};

struct InstanceData
{
    float4x4 World;
};