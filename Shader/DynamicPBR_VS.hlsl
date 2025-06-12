#include "Transform.fx"

#define MAX_BONES 100
cbuffer CB_SkinnedMatrix : register(b1)
{
    float4x4 Bones[MAX_BONES];
};

struct VSInput
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
    uint4 BoneID : BONEID;
    float4 Weight : WEIGHT;
    float4 Tangent : TANGENT; // 로컬 접선
    float4 Binormal : BINORMAL; // 로컬 이접선
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

VSOutput DynamicPBR_VS(VSInput _Input)
{
    VSOutput Output = (VSOutput) 0;

    // 1. 스키닝 매트릭스 계산
    float4x4 SkinMatrix = float4x4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        uint BoneIndex = _Input.BoneID[i];
        float Weight = _Input.Weight[i];
        if (Weight > 0.0f && BoneIndex < MAX_BONES)
        {
            SkinMatrix += Bones[BoneIndex] * Weight;
        }
    }

    // SkinNormal 계산
    float4 SkinPosition;
    SkinPosition.x = dot(SkinMatrix[0], _Input.Position); // row0 · [x y z 1]
    SkinPosition.y = dot(SkinMatrix[1], _Input.Position); // row1 · [x y z 1]
    SkinPosition.z = dot(SkinMatrix[2], _Input.Position); // row2 · [x y z 1]
    SkinPosition.w = dot(SkinMatrix[3], _Input.Position); // row3 · [x y z 1]
 
    float3 SkinNormal;
    {
        float3 Row0 = float3(SkinMatrix[0][0], SkinMatrix[0][1], SkinMatrix[0][2]);
        float3 Row1 = float3(SkinMatrix[1][0], SkinMatrix[1][1], SkinMatrix[1][2]);
        float3 Row2 = float3(SkinMatrix[2][0], SkinMatrix[2][1], SkinMatrix[2][2]);
        float3 LocalN = _Input.Normal.xyz;
        
        SkinNormal.x = dot(Row0, LocalN);
        SkinNormal.y = dot(Row1, LocalN);
        SkinNormal.z = dot(Row2, LocalN);
        SkinNormal = normalize(SkinNormal);
    }
    
    // SkinTangent 계산
    float3 SkinTangent;
    {
        float3 Row0 = float3(SkinMatrix[0][0], SkinMatrix[0][1], SkinMatrix[0][2]);
        float3 Row1 = float3(SkinMatrix[1][0], SkinMatrix[1][1], SkinMatrix[1][2]);
        float3 Row2 = float3(SkinMatrix[2][0], SkinMatrix[2][1], SkinMatrix[2][2]);
        float3 LocalT = _Input.Tangent.xyz;
        
        SkinTangent.x = dot(Row0, LocalT);
        SkinTangent.y = dot(Row1, LocalT);
        SkinTangent.z = dot(Row2, LocalT);
        SkinTangent = normalize(SkinTangent);
    }
    
    // SkinBinormal 계산
    float3 SkinBinormal;
    {
        float3 Row0 = float3(SkinMatrix[0][0], SkinMatrix[0][1], SkinMatrix[0][2]);
        float3 Row1 = float3(SkinMatrix[1][0], SkinMatrix[1][1], SkinMatrix[1][2]);
        float3 Row2 = float3(SkinMatrix[2][0], SkinMatrix[2][1], SkinMatrix[2][2]);
        float3 LocalB = _Input.Binormal.xyz;
        
        SkinBinormal.x = dot(Row0, LocalB);
        SkinBinormal.y = dot(Row1, LocalB);
        SkinBinormal.z = dot(Row2, LocalB);
        SkinBinormal = normalize(SkinBinormal);
    }
    
    // Position 설정
    float4 WorldPos = mul(SkinPosition, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
         
    // UV 좌표 설정
    Output.TexCoord = _Input.TexCoord.xy;
    
    // 월드 공간 기준으로 조명 계산을 진행하기 위해 WorldMatrix만 처리한 Position, Normal을 생성하여 Pixel Shader에 넘겨줌
    Output.WorldPosition = WorldPos.xyz;
    Output.WorldNormal = mul(SkinNormal, (float3x3) WorldMatrix);
    Output.WorldTangent = mul(SkinTangent, (float3x3) WorldMatrix);
    Output.WorldBinormal = mul(SkinBinormal, (float3x3) WorldMatrix);
    
    return Output;
}