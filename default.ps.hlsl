#include <default.common.hlsli>

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 lightDir = normalize(LightPos.xyz - input.WorldPos);
    float3 color = LightCL.xyz;
    float diffuse0 = saturate(dot(input.Normal, lightDir));
    float diffuse1 = saturate(dot(lightDir, float3(0, 1.F, 0.F)));
    float diffuse = (diffuse0 + diffuse1) / 2;
    return float4(color * diffuse * LightCL.w, 1.0F);
}
