cbuffer cbGameObject : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbCamera : register(b1)
{
    matrix viewMatrix;
    matrix projMatrix;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;
    position = mul(position, worldMatrix);
    position = mul(position, viewMatrix);
    position = mul(position, projMatrix);
    result.position = position;
    result.color = color;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}