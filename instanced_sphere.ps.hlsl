#include "sphere.common.hlsli"

Texture2D hsvMap : register(t0);
SamplerState samLinear : register(s0);

struct DS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

float4 main(DS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(LightPos.xyz - input.WorldPos);
    float diffuse = saturate(dot(input.Normal.xyz, lightDir) * 0.7);
    float4 color = hsvMap.Sample(samLinear, input.TexCoord);
    float3 finalColor = color.xyz * LightCL.xyz * LightCL.w * saturate(diffuse + 0.3F);
    
    return float4(finalColor.xyz, 1.0F);
}
