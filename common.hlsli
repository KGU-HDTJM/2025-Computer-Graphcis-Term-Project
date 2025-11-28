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

struct VertexData
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};
