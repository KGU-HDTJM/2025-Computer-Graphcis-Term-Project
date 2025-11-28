#include "common.hlsli"

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VertexData main(VertexData input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    return output;
}
