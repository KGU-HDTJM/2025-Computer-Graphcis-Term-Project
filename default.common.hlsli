cbuffer cbPerObject : register(b0)
{
    matrix World;
};

cbuffer cbPerFrame : register(b1)
{
    matrix View;
    float4 LightPos;
    float4 LightCL;
};


cbuffer cbProjection : register(b2)
{
    matrix Projection;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
    float3 WolrdPos : TEXCOORD2;
};
