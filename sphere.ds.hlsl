#include "common.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[4] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

[domain("quad")]
VertexData main(HS_CONSTANT_DATA_OUTPUT input, const OutputPatch<VertexData, 4> patch, float4 bary : SV_DomainLocation)
{
    VertexData output;

    // Barycentric 좌표로 보간
    float4 pos = patch[0].Position * bary.x +
                 patch[1].Position * bary.y +
                 patch[2].Position * bary.z +
                 patch[3].Position * bary.w;

    float4 normal = patch[0].Normal * bary.x +
                    patch[1].Normal * bary.y +
                    patch[2].Normal * bary.z +
                    patch[3].Normal * bary.w;

    float2 tex = patch[0].TexCoord * bary.x +
                 patch[1].TexCoord * bary.y +
                 patch[2].TexCoord * bary.z;

    // 구 형태로 변환
    pos.xyz = normalize(pos.xyz);

    // 행렬 변환
    float4 worldPos = mul(pos, World);
    float4 viewPos = mul(worldPos, View);
    output.Position = mul(viewPos, Projection);
    output.Normal = normalize(normal.xyz);
    output.TexCoord = tex;

    return output;
}
