#include "instanced_sphere.common.hlsli"

struct HS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
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
    
    output.World0 = patch[i].World0;
    output.World1 = patch[i].World1;
    output.World2 = patch[i].World2;
    output.World3 = patch[i].World3;
    
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
