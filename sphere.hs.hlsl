#include "sphere.common.hlsli"

struct HS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstFunc")]
HS_CONTROL_POINT_OUTPUT main(InputPatch<HS_INPUT, 3> patch, uint i : SV_OutputControlPointID)
{
    HS_CONTROL_POINT_OUTPUT output;
    output.Position = patch[i].Position;
    output.Normal = patch[i].Normal;
    output.TexCoord = patch[i].TexCoord;
    return output;
}

HS_CONSTANT_DATA_OUTPUT HSConstFunc(InputPatch<HS_INPUT, 3> patch)
{
    HS_CONSTANT_DATA_OUTPUT output;
    output.TessFactor[0] = TessFactor;
    output.TessFactor[1] = TessFactor;
    output.TessFactor[2] = TessFactor;
    output.InsideTessFactor = TessFactor;
    return output;
}
