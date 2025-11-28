#include "sphere.common.hlsli"

HS_CONSTANT_DATA_OUTPUT HS_ConstantFunc(InputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
    output.TessFactor[0] = TessFactor;
    output.TessFactor[1] = TessFactor;
    output.TessFactor[2] = TessFactor;
    output.TessFactor[3] = TessFactor;
    output.InsideTessFactor[0] = TessFactor;
    output.InsideTessFactor[1] = TessFactor;
    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_ConstantFunc")]
HS_CONTROL_POINT_OUTPUT main(InputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch, uint i : SV_OutputControlPointID)
{
    return patch[i];
}
