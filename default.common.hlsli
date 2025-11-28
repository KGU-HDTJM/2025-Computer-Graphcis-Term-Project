cbuffer WorldMatrixCB : register(b0)
{
    matrix World;
};

cbuffer ViewMatrixCB : register(b1)
{
    matrix View;
};

cbuffer ProjectionMatrixCB : register(b2)
{
    matrix Projection;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
};
