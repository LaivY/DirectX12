cbuffer cbGameObject : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbCamera : register(b1)
{
    matrix viewMatrix;
    matrix projMatrix;
}

////////////////////////////////////////////////////////

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

////////////////////////////////////////////////////////

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

////////////////////////////////////////////////////////

VSOutput VSMain(VSInput input)
{
    VSOutput result;
    result.position = mul(input.position, worldMatrix);
    result.position = mul(result.position, viewMatrix);
    result.position = mul(result.position, projMatrix);
    result.uv = input.uv;
    return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}