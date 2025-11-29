#include "sphere.common.hlsli"

float4 main(DS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float diffuse = saturate(dot(input.Normal.xyz, lightDir));
    //return float4(1.F, 1.F, 1.F, 1.F);
    return float4(diffuse, diffuse * 0.5f, diffuse * 0.2f, 1.0f);
}
