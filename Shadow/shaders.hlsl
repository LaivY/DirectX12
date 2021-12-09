#include "common.hlsl"

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.positionW = mul(input.position, worldMatrix);
    output.positionH = mul(output.positionW, viewMatrix);
    output.positionH = mul(output.positionH, projMatrix);    
    output.color = input.color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return input.color;
}

// --------------------------------------

PS_INPUT VSTextureMain(VS_INPUT input)
{
    PS_INPUT output;
    output.positionH = mul(input.position, worldMatrix);
    output.positionH = mul(output.positionH, viewMatrix);
    output.positionH = mul(output.positionH, projMatrix);
    output.uv0 = input.uv0;
    return output;
}

float4 PSTextureMain(PS_INPUT input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv0);
}

// --------------------------------------

VS_INPUT VSBillboardMain(VS_INPUT input)
{
    VS_INPUT output;
    output.position = mul(input.position, worldMatrix);
    output.uv0 = input.uv0;
    return output;
}

[maxvertexcount(4)]
void GSBillboardMain(point VS_INPUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
    // y축으로만 회전하는 빌보드
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = eye - input[0].position.xyz;
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up, look);
    
    float hw = 0.5f * input[0].uv0.x;
    float hh = 0.5f * input[0].uv0.y;
    
    float4 position[4] =
    {
        float4(input[0].position.xyz + (hw * right) - (hh * up), 1.0f), // LB
        float4(input[0].position.xyz + (hw * right) + (hh * up), 1.0f), // LT
        float4(input[0].position.xyz - (hw * right) - (hh * up), 1.0f), // RB
        float4(input[0].position.xyz - (hw * right) + (hh * up), 1.0f)  // RT
    };
    
    float2 uv[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    PS_INPUT output;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        // 모든 값들을 설정해줘야 오류가 나지않음
        output.positionW = position[i];
        output.positionH = mul(output.positionW, viewMatrix);
        output.positionH = mul(output.positionH, projMatrix);
        output.normalW = float3(0.0f, 0.0f, 0.0f);
        output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        output.uv0 = uv[i];
        output.uv1 = float2(0.0f, 0.0f);
        triStream.Append(output);
    }
}

float4 PSBillboardMain(PS_INPUT input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv0);
}

// --------------------------------------

PS_INPUT VSTerrainMain(VS_INPUT input)
{
    PS_INPUT output;
    output.positionH = mul(input.position, worldMatrix);
    output.positionH = mul(output.positionH, viewMatrix);
    output.positionH = mul(output.positionH, projMatrix);
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;
    return output;
}

float4 PSTerrainMain(PS_INPUT input) : SV_TARGET
{
    float4 baseTextureColor = g_texture.Sample(g_sampler, input.uv0);
    float4 detailTextureColor = g_detailTexture.Sample(g_sampler, input.uv1);
    return lerp(baseTextureColor, detailTextureColor, 0.4f);
}

// --------------------------------------

VS_INPUT VSTerrainTessMain(VS_INPUT input)
{
    VS_INPUT output;
    output.position = mul(input.position, worldMatrix);
    output.normal = mul(float4(input.normal, 0.0f), worldMatrix).xyz;
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;
    return output;
}

float CalculateTessFactor(float3 f3Position)
{
    float fDistToCamera = distance(f3Position, eye);
    float s = saturate(fDistToCamera / 75.0f);
    return lerp(64.0f, 3.0f, s);
}

PatchTess TerrainTessConstantHS(InputPatch<VS_INPUT, 25> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess output;
    output.EdgeTess[0] = CalculateTessFactor(0.5f * (patch[0].position.xyz + patch[4].position.xyz));
    output.EdgeTess[1] = CalculateTessFactor(0.5f * (patch[0].position.xyz + patch[20].position.xyz));
    output.EdgeTess[2] = CalculateTessFactor(0.5f * (patch[4].position.xyz + patch[24].position.xyz));
    output.EdgeTess[3] = CalculateTessFactor(0.5f * (patch[20].position.xyz + patch[24].position.xyz));
    
    float3 center = float3(0, 0, 0);
    for (int i = 0; i < 25; ++i)
        center += patch[i].position.xyz;
    center /= 25.0f;
    output.InsideTess[0] = output.InsideTess[1] = CalculateTessFactor(center);
    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("TerrainTessConstantHS")]
[maxtessfactor(64.0f)]
VS_INPUT HSTerrainTessMain(InputPatch<VS_INPUT, 25> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    VS_INPUT output;
    output.position = p[i].position;
    output.normal = p[i].normal;
    output.uv0 = p[i].uv0;
    output.uv1 = p[i].uv1;
    return output;
}

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
    float tInv = 1.0f - t;
    fBernstein[0] = tInv * tInv * tInv * tInv;
    fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
    fBernstein[2] = 6.0f * t * t * tInv * tInv;
    fBernstein[3] = 4.0f * t * t * t * tInv;
    fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<VS_INPUT, 25> patch, float2 uv)
{
    // 4차 베지에 곡선 계수 계산
    float uB[5], vB[5];
    BernsteinCoeffcient5x5(uv.x, uB);
    BernsteinCoeffcient5x5(uv.y, vB);
    
    // 4차 베지에 곡면 계산
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 5; ++i)
    {
        float3 subSum = float3(0.0f, 0.0f, 0.0f);
        for (int j = 0; j < 5; ++j)
        {
            subSum += uB[j] * patch[i * 5 + j].position;
        }
        sum += vB[i] * subSum;
    }
    return sum;
}

[domain("quad")]
PS_INPUT DSTerrainTessMain(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<VS_INPUT, 25> patch)
{
    PS_INPUT output;
    
    // 위치 좌표(베지에 곡면)
    output.positionW = float4(CubicBezierSum5x5(patch, uv), 1.0f);
    output.positionH = mul(output.positionW, viewMatrix);
    output.positionH = mul(output.positionH, projMatrix);
    
    // 노말 보간
    output.normalW = lerp(lerp(patch[0].normal, patch[4].normal, uv.x), lerp(patch[20].normal, patch[24].normal, uv.x), uv.y);
    
    // 텍스쳐 좌표 보간
    output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
    output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);
    
    return output;
}

float4 PSTerrainTessMain(PS_INPUT input) : SV_TARGET
{
    float4 baseTextureColor = g_texture.Sample(g_sampler, input.uv0);
    float4 detailTextureColor = g_detailTexture.Sample(g_sampler, input.uv1);
    float4 texColor = lerp(baseTextureColor, detailTextureColor, 0.4f);
    
    // 그림자 계산
    float4 shadowPosH = mul(input.positionW, lightViewMatrix);
    shadowPosH = mul(shadowPosH, lightProjMatrix);
    shadowPosH = mul(shadowPosH, NDCToTextureMatrix);
    float shadowFacter = CalcShadowFactor(shadowPosH);
    
    return texColor * shadowFacter;
}

float4 PSTerrainTessWireMain(PS_INPUT pin) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

// --------------------------------------

PS_INPUT VSShadowMain(VS_INPUT input)
{
    PS_INPUT output;
    output.positionW = mul(input.position, worldMatrix);
    output.positionH = mul(output.positionW, lightViewMatrix);
    output.positionH = mul(output.positionH, lightProjMatrix);
    return output;
}

// --------------------------------------

PS_INPUT VSModelMain(VS_INPUT input)
{
    PS_INPUT output;
    output.positionW = mul(input.position, worldMatrix);
    output.positionH = mul(output.positionW, viewMatrix);
    output.positionH = mul(output.positionH, projMatrix);
    output.normalW = mul(float4(input.normal, 0.0f), worldMatrix).xyz;
    output.color = input.color;
    return output;
}

float4 PSModelMain(PS_INPUT input) : SV_TARGET
{
    // 노말 벡터 정규화
    input.normalW = normalize(input.normalW);
    
    // 조명 -> 눈 단위 벡터
    float3 toEye = normalize(eye - input.positionW.xyz);
    
    // 간접 조명(씬 전체에 비치는 간접광이 있어야되는데... 나는 안했음)
    float4 ambient = /* gAmbientLight */materials[0].diffuseAlbedo;
    
    // 최종 조명 색깔
    float4 litColor = ambient;
    
    // 모든 조명 계산
    for (int i = 0; i < MAX_LIGHT; ++i)
    {
        if (lights[i].type == DIRECTIONAL_LIGHT)
        {
            litColor += float4(ComputeDirectionalLight(lights[0], materials[0], input.normalW, toEye), 0.0f);
        }
        else if (lights[i].type == POINT_LIGHT)
        {
            /* 점 조명 처리하는 코드 */
        }
    }
    
    litColor.a = materials[0].diffuseAlbedo.a;
    return litColor;
}