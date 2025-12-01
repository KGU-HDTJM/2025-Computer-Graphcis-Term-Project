#include <default.common.hlsli>

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 light = float4(0.F, 1.F, 0.F, 0.F);
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    float diffuse = dot(input.Normal, light.xyz);
    
    return color * diffuse;
}
