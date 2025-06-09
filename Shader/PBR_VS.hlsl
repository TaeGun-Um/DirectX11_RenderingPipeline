#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT; // ���� ����
    float4 Binormal : BINORMAL; // ���� ������
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

VSOutput PBR_VS(VSInput _Input)
{
    VSOutput Output;
    
    // Position ����
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
    
    // ���� ���� ���� �븻, ����, ������ ���ϱ�
    Output.WorldNormal = mul(float4(_Input.Normal.xyz, 0.0f), WorldMatrix).rgb;
    Output.WorldTangent = mul(float4(_Input.Tangent.xyz, 0.0f), WorldMatrix).rgb;
    Output.WorldBinormal = mul(float4(_Input.Binormal.xyz, 0.0f), WorldMatrix).rgb;
    
    // UV ��ǥ ����
    Output.TexCoord = _Input.TexCoord;
    
    // ���� ���� ���� Position ���ϱ�
    Output.WorldPosition = mul(float4(_Input.Position.xyz, 1.0f), WorldMatrix).rgb;
   
    return Output;
}