#include <default.common.hlsli>

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(LightPos.xyz - input.WorldPos);
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    float diffuse = saturate(dot(input.Normal, lightDir));
    
    return color * diffuse;
}
