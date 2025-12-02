#include "sphere.common.hlsli"

Texture2D hsvMap : register(t0);
SamplerState samLinear : register(s0);

float4 main(DS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(input.WorldPos - LightPos.xyz);
    float diffuse = saturate(dot(input.Normal.xyz, lightDir));
    float4 color = hsvMap.Sample(samLinear, input.TexCoord);
    float3 finalColor = color.xyz * LightCL.xyz * LightCL.w * diffuse;
    
    return float4(color.xyz, diffuse);
}
