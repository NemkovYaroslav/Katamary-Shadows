cbuffer CameraConstantBuffer : register(b0)
{
    row_major matrix viewProjection;
    row_major matrix model;
    float3 cameraPosition;
};

struct RemLight
{
    float3 direction;
    float3 ambient;
    float3 diffuse;
    float3 specular;
};
cbuffer LightConstantBuffer : register(b1)
{
	RemLight remLight;
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
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
    float3 modelPos : POSITION;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;

    float4 pos = float4(input.pos, 1.0f);
    float4 modelPos = mul(pos, model);
    output.pos = mul(modelPos, viewProjection);
    output.tex = input.tex;
    output.normal = mul(transpose(model), input.normal);
    output.modelPos = modelPos.xyz;
    
    return output;
}

Texture2D DiffuseMap;
SamplerState Sampler;

float3 CalcDirLight(RemLight remLight, float3 normal, float3 viewDir, float2 tex);

float4 PSMain(PS_IN input) : SV_Target
{
    float3 norm    = normalize(input.normal);
    float3 viewDir = normalize(cameraPosition - input.modelPos);

    float3 result  = CalcDirLight(remLight, norm, viewDir, input.tex);

    return float4(result, 1.0f);
}

float3 CalcDirLight(RemLight remLight, float3 normal, float3 viewDir, float2 tex)
{
    float3 diffValue = DiffuseMap.Sample(Sampler, tex).rgb;
    
    float3 lightDir = normalize( - remLight.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    float3 reflectDir = reflect( - lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);

    float3 ambient  = remLight.ambient         * diffValue;
    float3 diffuse  = remLight.diffuse  * diff * diffValue;
    float3 specular = remLight.specular * spec * diffValue;
    
    return (ambient + diffuse + specular);
}