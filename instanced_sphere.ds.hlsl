#include "instanced_sphere.common.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

struct DS_CONTROL_POINT_INPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

struct DS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};


[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, const OutputPatch<DS_CONTROL_POINT_INPUT, 3> patch, float3 bary : SV_DomainLocation)
{
    DS_OUTPUT output;

    float3 pos = patch[0].Position * bary.x + patch[1].Position * bary.y + patch[2].Position * bary.z;
    float3 normal = patch[0].Normal * bary.x + patch[1].Normal * bary.y + patch[2].Normal * bary.z;
    
    pos = normalize(pos);
    normal = normalize(normal);
    float4x4 world = float4x4(patch[0].World0, patch[0].World1, patch[0].World2, patch[0].World3);
    
    float4 worldPos = mul(float4(pos, 1.0f), world);
    float4 viewPos = mul(worldPos, View);
    float4 projPos = mul(viewPos, Projection);
    
    output.Position = projPos;
    output.Normal = normalize(mul(float4(normal, 0.F), world));
    output.TexCoord = patch[0].TexCoord;
    output.WorldPos = worldPos.xyz;

    return output;
}
