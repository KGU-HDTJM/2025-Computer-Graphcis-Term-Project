#include "sphere.common.hlsli"

Texture2D hsvMap : register(t0);
SamplerState samLinear : register(s0);

float4 main(DS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float diffuse = saturate(dot(input.Normal.xyz, lightDir));
    float4 color = hsvMap.Sample(samLinear, input.TexCoord) * diffuse;
    color.w = 1.0F;
    return color;
}
