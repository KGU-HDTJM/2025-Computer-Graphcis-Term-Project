#include <default.common.hlsli>

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(input.WolrdPos - LightPos.xyz);
    float diffuse = saturate(dot(input.Normal.xyz, lightDir));
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    
    return color * diffuse;
}
