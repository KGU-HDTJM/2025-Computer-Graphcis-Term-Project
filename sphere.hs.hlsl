#include "common.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[4] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT HS_ConstantFunc(InputPatch<VertexData, 4> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
    output.TessFactor[0] = TessFactor;
    output.TessFactor[1] = TessFactor;
    output.TessFactor[2] = TessFactor;
    output.TessFactor[3] = TessFactor;
    output.InsideTessFactor = TessFactor;
    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_ConstantFunc")]
VertexData main(InputPatch<VertexData, 3> patch, uint i : SV_OutputControlPointID)
{
    return patch[i];
}
