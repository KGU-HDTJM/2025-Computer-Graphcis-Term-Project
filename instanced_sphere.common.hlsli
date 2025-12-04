#include "sphere.common.hlsli"

cbuffer CBCompute : register(b4)
{
    uint FrameOffset; // Rotating offset
    uint InstanceCount; // Total objects
    float DeltaTime;
    float ComputePadding;
}

struct InstanceData
{
    float4x4 World;
};