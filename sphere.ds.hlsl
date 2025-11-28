#include "sphere.common.hlsli"

[domain("quad")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch, float2 uv : SV_DomainLocation)
{
    DS_OUTPUT output;

    // Bilinear interpolation
    float4 pos = lerp(lerp(patch[0].Position, patch[1].Position, uv.x),
                      lerp(patch[2].Position, patch[3].Position, uv.x), uv.y);

    float4 normal = lerp(lerp(patch[0].Normal, patch[1].Normal, uv.x),
                         lerp(patch[2].Normal, patch[3].Normal, uv.x), uv.y);

    float2 tex = lerp(lerp(patch[0].TexCoord, patch[1].TexCoord, uv.x),
                      lerp(patch[2].TexCoord, patch[3].TexCoord, uv.x), uv.y);

    // Sphere projection
    pos.xyz = normalize(pos.xyz);

    // Transform
    float4 worldPos = mul(pos, World);
    float4 viewPos = mul(worldPos, View);
    output.Position = mul(viewPos, Projection);
    output.Normal = normalize(normal.xyz);
    output.TexCoord = tex;

    return output;
}
