#include "sphere.common.hlsli"

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    return output;
}
