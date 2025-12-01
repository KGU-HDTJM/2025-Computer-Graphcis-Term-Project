
// 월드, 뷰, 프로젝션 매트릭스
cbuffer cbPerObject : register(b0)
{
    matrix World;
};

cbuffer cbPerFrame : register(b1)
{
    matrix View;
};

cbuffer cbProjection : register(b2)
{
    matrix Projection;
};

// 테셀레이션 팩터
cbuffer TessellationCB : register(b3)
{
    float TessFactor;
    float3 Padding;
};

struct DS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};
