#include "instanced_sphere.common.hlsli"


struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 World0 : WORLD0;
    float4 World1 : WORLD1;
    float4 World2 : WORLD2;
    float4 World3 : WORLD3;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 World0 : TEXCOORD1;
    float4 World1 : TEXCOORD2;
    float4 World2 : TEXCOORD3;
    float4 World3 : TEXCOORD4;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.World0 = input.World0;
    output.World1 = input.World1;
    output.World2 = input.World2;
    output.World3 = input.World3;

    output.Position = input.Position;
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    
    return output;
}
