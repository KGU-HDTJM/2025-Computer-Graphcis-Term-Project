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

// 테셀레이션 팩터
cbuffer TessellationCB : register(b3)
{
    float TessFactor;
    float3 TessCBPadding;
};
