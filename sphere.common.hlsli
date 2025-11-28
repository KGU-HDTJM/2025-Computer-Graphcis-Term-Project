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

cbuffer TessellationCB : register(b3)
{
    float TessFactor;
    float3 Padding;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};

struct DS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
};
