#include "instanced_sphere.common.hlsli"

struct ComputeBuf
{
    float4 Velocity;
    float4 Position;
    float Radius;
    float3 Padding;
};

StructuredBuffer<ComputeBuf> reaadBuffer : register(t0);
RWStructuredBuffer<ComputeBuf> computeBuffer : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    ComputeBuf data = computeBuffer[id];
    
    data.Position.xyz += data.Velocity.xyz * DeltaTime;
    
    bool bCollisionDetected = false;
    float4 newVelocity = float4(0.F, 0.F, 0.F, 0.F);
    
    for (uint i = 0; i < InstanceCount; ++i)
    {
        if (id == i)
        {
            continue;
        }
        ComputeBuf temp = computeBuffer[i];
        
        float3 diff = data.Position.xyz - temp.Position.xyz;
        float radiusSum = data.Radius + temp.Radius;
        float distanceSquare = dot(diff, diff);
        if (distanceSquare < radiusSum * radiusSum)
        {
            bCollisionDetected = true;
            newVelocity += temp.Velocity;
        }
    }
    if (bCollisionDetected)
    {
        data.Velocity = newVelocity;
    }
    computeBuffer[id] = data;
}
