#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION0;
    float3 WorldViewPosition : POSITION1;
    float3 WorldNormal : NORMAL0;
    float3 WorldViewNormal : NORMAL1;
};

struct PSOutPut
{
    float4 MeshTarget : SV_TARGET0;
    float4 WPositionTarget : SV_TARGET1; // World
    float4 WVPositionTarget : SV_TARGET2; // WorldView
    float4 WNormalTarget : SV_TARGET3; // World
    float4 WVNormalTarget : SV_TARGET4; // WorldView
};

 // �� ���Ϳ� normalize�� ���ִ� ������, ��������� normalize�� ���͸� �־���� �ϴ��� 
 // ������ ���� ��� ������ �𸣱� ������ �׳� ���ִ°�(�������� ���� ó����� ���� ��)
PSOutPut Grapics_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut Output = (PSOutPut) 0;
    
    Output.MeshTarget = BaseColorTex.Sample(Sampler, _Input.TexCoord); // �ؽ����÷�
    Output.WPositionTarget = float4(_Input.WorldPosition.xyz, 1.0f); // ���彺���̽� Position
    Output.WVPositionTarget = float4(_Input.WorldViewPosition.xyz, 1.0f); // �佺���̽� Position
    Output.WNormalTarget = float4(_Input.WorldNormal, 1.0f); // ���彺���̽� Normal
    Output.WVNormalTarget = float4(_Input.WorldViewNormal, 1.0f); // �佺���̽� Normal

    return Output;
}