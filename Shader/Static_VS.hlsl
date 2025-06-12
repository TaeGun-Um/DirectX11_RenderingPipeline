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
    
    // Position ����
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    OutPut.Position = mul(ViewPos, ProjectionMatrix);
    
    // UV ��ǥ ����
    OutPut.TexCoord = _Input.TexCoord;
    
    // ���� ���� �������� ���� ����� �����ϱ� ���� WorldMatrix�� ó���� Position, Normal�� �����Ͽ� Pixel Shader�� �Ѱ���
    OutPut.WorldPosition = mul(float4(_Input.Position.xyz, 1.0f), WorldMatrix).xyz;
    
    float3 WorldNorm = mul(_Input.Normal.xyz, (float3x3) WorldMatrix);
    OutPut.WorldNormal = WorldNorm;
    
    return OutPut;
}