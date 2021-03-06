cbuffer cbGameObject : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbCamera : register(b1)
{
    matrix viewMatrix;
    matrix projMatrix;
}

// --------------------------------------

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

struct VSTerrainInput
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VSTerrainOutput
{
    float4 position : SV_POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

// --------------------------------------

Texture2D g_texture : register(t0);
Texture2D g_detailTexture : register(t1);
SamplerState g_sampler : register(s0);

// --------------------------------------

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projMatrix);
    output.uv = input.uv;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}

// --------------------------------------

VSTerrainOutput VSTerrainMain(VSTerrainInput input)
{
    VSTerrainOutput output;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projMatrix);
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;
    return output;
}

float4 PSTerrainMain(VSTerrainOutput input) : SV_TARGET
{
    float4 baseTextureColor = g_texture.Sample(g_sampler, input.uv0);
    float4 detailTextureColor = g_detailTexture.Sample(g_sampler, input.uv1);
    return saturate(baseTextureColor * 0.6f + detailTextureColor * 0.4f);
}