#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

struct PSOutPut
{
    float4 MeshTarget : SV_TARGET0;
    float4 PositionTarget : SV_TARGET1; // World Position Target
    float4 NormalTarget : SV_TARGET2; // World Normal Target
};

 // �� ���Ϳ� normalize�� ���ִ� ������, ��������� normalize�� ���͸� �־���� �ϴ��� 
 // ������ ���� ��� ������ �𸣱� ������ �׳� ���ִ°�(�������� ���� ó����� ���� ��)
PSOutPut Graphics_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut Output = (PSOutPut) 0;
    
    Output.MeshTarget = BaseColorTex.Sample(Sampler, _Input.TexCoord); // �ؽ����÷�
    Output.PositionTarget = float4(_Input.WorldPosition.xyz, 1.0f); // ���彺���̽� Position
    Output.NormalTarget = float4(_Input.WorldNormal, 1.0f); // ���彺���̽� Normal

    return Output;
}