#include "LightData.fx"

Texture2D BaseColorTex : register(t0);
Texture2D NormalTex : register(t1);
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

struct PSOutPut
{
    float4 MeshTarget : SV_TARGET0;
    float4 PositionTarget : SV_TARGET1; // World Position
    float4 NormalTarget : SV_TARGET2; // World Normal
    float4 TangentTarget : SV_TARGET3; // World Tangent
    float4 BinormalTarget : SV_TARGET4; // World Binormal
};

// �� ���Ϳ� normalize�� ���ִ� ������, ��������� normalize�� ���͸� �־���� �ϴ��� 
// ������ ���� ��� ������ �𸣱� ������ �׳� ���ִ°�(�������� ���� ó����� ���� ��)
PSOutPut PBR_PS(PSInput _Input) : SV_TARGET
{
    //// �ؽ�ó ����
    //float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord.xy);
    
    //// ������� ���� �ȼ�(ǥ��) ��ġ�� ���� ���� ����
    //float3 PixelPosition = _Input.WorldPosition;
    
    //// �븻 �� ���ø� (Tangent-space �븻 �� [-1, 1] ����)
    //float3 SampledNormalTS = NormalTex.Sample(Sampler, _Input.TexCoord.xy).xyz * 2.0f - 1.0f;
    //SampledNormalTS = normalize(SampledNormalTS);
    
    //// TBN ��Ʈ���� ���� (��(column) ������ ����)
    //float3x3 TBNMatrix;
    //TBNMatrix[0] = normalize(_Input.WorldTangent); // T
    //TBNMatrix[1] = normalize(_Input.WorldBinormal); // B
    //TBNMatrix[2] = normalize(_Input.WorldNormal); // N
    
    //float3 MappedWorldNormal = normalize(mul(SampledNormalTS, TBNMatrix));
    
    PSOutPut Output = (PSOutPut) 0;
    
    Output.MeshTarget = BaseColorTex.Sample(Sampler, _Input.TexCoord); // �ؽ����÷�
    Output.PositionTarget = float4(_Input.WorldPosition.xyz, 1.0f); // ���彺���̽� Position
    Output.NormalTarget = float4(_Input.WorldNormal, 1.0f); // ���彺���̽� Normal
    Output.TangentTarget = float4(_Input.WorldTangent, 1.0f); // ���彺���̽� Normal
    Output.BinormalTarget = float4(_Input.WorldBinormal, 1.0f); // ���彺���̽� Normal

    return Output;
}