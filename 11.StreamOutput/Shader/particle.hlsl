#include "common.hlsl"

struct VS_PARTICLE_INPUT
{
    float3 position : POSITION;
    float2 size     : SIZE;
    float lifeTime  : LIFETIME;
    float age       : AGE;
};

struct GS_PARTICLE_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float lifeTime  : LIFETIME;
    float age       : AGE;
};

VS_PARTICLE_INPUT VSParticleMain(VS_PARTICLE_INPUT input)
{
    // GS로 넘겨주는 역할을 하는 VS
    return input;
}

[maxvertexcount(1)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
    VS_PARTICLE_INPUT particle = input[0];
    
    // 나이 증가
    particle.age += g_deltaTime;
    
    // 수명이 다했다면 나이와 위치를 재설정해줌
    if (particle.age > particle.lifeTime)
    {
        particle.age = 0;
        particle.position += float3(0.0f, 100.0f, 0.0f);
    }
    else
    {
        // 수명이 남았다면 밑으로 떨어짐
        particle.position -= float3(0.0f, 10.0f * g_deltaTime, 0.0f);
    }
    output.Append(particle);
}

[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_INPUT input[1], inout TriangleStream<GS_PARTICLE_OUTPUT> outputStream)
{
    // 정점의 월드좌표계에서의 좌표
    float3 positionW = mul(float4(input[0].position, 1.0f), worldMatrix).xyz;
    
    // y축으로만 회전하는 빌보드
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = eye - positionW;
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up, look);
    
    float hw = 0.5f * input[0].size.x;
    float hh = 0.5f * input[0].size.y;
    
    float4 position[4] =
    {
        float4(positionW + (hw * right) - (hh * up), 1.0f), // LB
        float4(positionW + (hw * right) + (hh * up), 1.0f), // LT
        float4(positionW - (hw * right) - (hh * up), 1.0f), // RB
        float4(positionW - (hw * right) + (hh * up), 1.0f)  // RT
    };
    
    float2 uv[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    GS_PARTICLE_OUTPUT output = (GS_PARTICLE_OUTPUT)0;
    output.lifeTime = input[0].lifeTime;
    output.age = input[0].age;
    
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.position = mul(position[i], viewMatrix);
        output.position = mul(output.position, projMatrix);
        output.uv = uv[i];
        outputStream.Append(output);
    }
}

float4 PSParticleMain(GS_PARTICLE_OUTPUT input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}