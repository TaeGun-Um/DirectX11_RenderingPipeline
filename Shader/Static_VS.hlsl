#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VSOutput Static_VS(VSInput _Input)
{
    VSOutput Output;
    
    // Position 설정
    float4 WorldPosition = mul(_Input.Position, WorldMatrix);
    float4 ViewPosition = mul(WorldPosition, ViewMatrix);
    Output.Position = mul(ViewPosition, ProjectionMatrix);
    
    // UV 좌표 설정
    Output.TexCoord = _Input.TexCoord;
    
    // 월드 공간 기준으로 조명 계산을 진행하기 위해 WorldMatrix만 처리한 Position, Normal을 생성하여 Pixel Shader에 넘겨줌
    Output.WorldPosition = mul(float4(_Input.Position.xyz, 1.0f), WorldMatrix).rgb;
    Output.WorldNormal = mul(float4(_Input.Normal.xyz, 0.0f), WorldMatrix).rgb;
   
    return Output;
}