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
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

VSOutput Static_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;
    
    // Position 설정
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    OutPut.Position = mul(ViewPos, ProjectionMatrix);
    
    // UV 좌표 설정
    OutPut.TexCoord = _Input.TexCoord;
    
    // 월드 공간 기준으로 조명 계산을 진행하기 위해 WorldMatrix만 처리한 Position, Normal을 생성하여 Pixel Shader에 넘겨줌
    OutPut.WorldPosition = mul(float4(_Input.Position.xyz, 1.0f), WorldMatrix).xyz;
    
    float3 WorldNorm = mul(_Input.Normal.xyz, (float3x3) WorldMatrix);
    OutPut.WorldNormal = WorldNorm;
    
    return OutPut;
}