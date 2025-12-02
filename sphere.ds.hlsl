#include "sphere.common.hlsli"

struct DS_CONTROL_POINT_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, const OutputPatch<DS_CONTROL_POINT_INPUT, 3> patch, float3 bary : SV_DomainLocation)
{
    DS_OUTPUT output;

    // 위치 보간
    float3 pos = patch[0].Position * bary.x + patch[1].Position * bary.y + patch[2].Position * bary.z;
    float3 normal = patch[0].Normal * bary.x + patch[1].Normal * bary.y + patch[2].Normal * bary.z;
    float2 tex = patch[0].TexCoord * bary.x + patch[1].TexCoord * bary.y + patch[2].TexCoord * bary.z;

    // Sphere 형태로 정규화
    pos = normalize(pos);
    normal = normalize(normal);

    // 월드 변환
    float4 worldPos = mul(float4(pos, 1.0f), World);
    float4 viewPos = mul(worldPos, View);
    float4 projPos = mul(viewPos, Projection);

    output.Position = projPos;
    output.Normal = normalize(mul(float4(normal, 0.0f), World)); // Normal 변환
    output.TexCoord = tex;
    output.WorldPos = output.Position.xyz;
    return output;
}
