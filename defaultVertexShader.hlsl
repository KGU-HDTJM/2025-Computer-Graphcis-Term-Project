#include <common.hlsli>

VertexData main(VertexData input)
{
    VertexData output = (VertexData) 0;

    input.Position.y *= 5;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = float4(normalize(mul(input.Normal.xyz, (float3x3)World)), 0.0);
    output.TexCoord = input.tex;


    return output;
}