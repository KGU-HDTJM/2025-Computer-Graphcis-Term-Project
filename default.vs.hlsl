#include <default.common.hlsli>

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    
    output.Position = mul(input.Position, World);
    output.Position.xz *= 4;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = normalize(mul(input.Normal.xyz, (float3x3)World));
    output.TexCoord = input.TexCoord;
    output.WolrdPos = output.Position.xyz;
    return output;
}