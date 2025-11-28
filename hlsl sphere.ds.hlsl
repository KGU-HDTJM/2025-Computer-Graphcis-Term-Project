#include "common.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[4] : SV_TessFactor;          // quad -> 4 outer tess factors
    float2 InsideTessFactor : SV_InsideTessFactor; // quad -> 2 inside tess factors
};

[domain("quad")]
VertexData main(HS_CONSTANT_DATA_OUTPUT input, const OutputPatch<VertexData, 4> patch, float2 uv : SV_DomainLocation)
{
    VertexData output;

    // Bilinear (quad) interpolation using UV from SV_DomainLocation
    float u = uv.x;
    float v = uv.y;
    float w00 = (1.0 - u) * (1.0 - v); // patch[0]
    float w10 = u * (1.0 - v);         // patch[1]
    float w11 = u * v;                 // patch[2]
    float w01 = (1.0 - u) * v;         // patch[3]

    float4 pos = patch[0].Position * w00 +
                 patch[1].Position * w10 +
                 patch[2].Position * w11 +
                 patch[3].Position * w01;

    float4 normal = patch[0].Normal * w00 +
                    patch[1].Normal * w10 +
                    patch[2].Normal * w11 +
                    patch[3].Normal * w01;

    float2 tex = patch[0].TexCoord * w00 +
                 patch[1].TexCoord * w10 +
                 patch[2].TexCoord * w11 +
                 patch[3].TexCoord * w01;

    // Project onto unit sphere
    pos.xyz = normalize(pos.xyz);
    pos.w = 1.0; // ensure correct homogeneous coordinate for matrix mul

    // Transform and output
    float4 worldPos = mul(pos, World);
    float4 viewPos = mul(worldPos, View);
    output.Position = mul(viewPos, Projection);
    output.Normal = normalize(normal.xyz);
    output.TexCoord = tex;

    return output;
}