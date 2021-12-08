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
}

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
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float4 color    : COLOR;
};