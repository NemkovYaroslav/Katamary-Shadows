cbuffer CameraConstantBuffer : register(b0)
{
    row_major matrix viewProjection;
    row_major matrix model;
};

struct VS_IN
{
    float3 pos : POSITION0;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    //float2 tex : TEXCOORD;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;

    float4 pos = float4(input.pos, 1.0f);
    float4 modelPos = mul(pos, model);
    output.pos = mul(modelPos, viewProjection);
    //output.tex = input.tex;
    
    return output;
}

float PSMain(PS_IN input) : SV_Depth
{
    float depth = input.pos.z / input.pos.w;
    return depth;
}