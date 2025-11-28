cbuffer cbNeverChanges : register(b0)
{
    matrix View;
};

cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
};


struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Nor : NORMAL;
    float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Nor : NORMAL;
    float2 tex : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0; 

    input.Pos.y *= 5;
    output.Pos = mul(input.Pos , World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Nor = float4(normalize(mul(input.Nor.xyz, (float3x3)World)), 0.0);
    output.tex = input.tex;


    return output;
}