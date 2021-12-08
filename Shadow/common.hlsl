#include "lighting.hlsl"
#define MAX_LIGHT       1
#define MAX_MATERIAL    1

// --------------------------------------

cbuffer cbGameObject : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbCamera : register(b1)
{
    matrix viewMatrix;
    matrix projMatrix;
    float3 eye;
}

cbuffer cbScene : register(b2)
{
    Light lights[MAX_LIGHT];
    Material materials[MAX_MATERIAL];
    
    matrix lightViewMatrix;
    matrix lightProjMatrix;
}

Texture2D g_texture                     : register(t0);
Texture2D g_detailTexture               : register(t1);
Texture2D g_shadowMap                   : register(t2);
SamplerState g_sampler                  : register(s0);
SamplerComparisonState g_shadowSampler  : register(s1);

// --------------------------------------

struct VSInput
{
    float4 position : POSITION;
    float4 color    : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

struct VSBillboardInput
{
    float4 position : POSITION;
    float2 size     : SIZE;
};

struct VSBillboardOutput
{
    float4 position : POSITION;
    float2 size     : SIZE;
};

struct GSBillboardOutput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

struct VSTextureInput
{
    float4 position : POSITION;
    float2 uv       : TEXCOORD;
};

struct VSTextureOutput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

struct VSTerrainInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct VSTerrainOutput
{
    float4 position : SV_POSITION;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct PatchTess
{
    float EdgeTess[4]   : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

struct HSOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct VSModelInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float4 color    : COLOR;
};

struct VSModelOutput
{
    float4 positionH    : SV_POSITION;
    float4 positionW    : POSITION0;
    float3 normal       : NORMAL;
    float4 color        : COLOR;
};

// --------------------------------------

float CalcShadowFactor(float4 shadowPosH)
{   
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    g_shadowMap.GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += g_shadowMap.SampleCmpLevelZero(g_shadowSampler, shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / 9.0f;
}

