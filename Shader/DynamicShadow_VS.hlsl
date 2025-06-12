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
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 WVPosition : POSITION0;
    float4 WVPPosition : POSITION1;
};

VSOutput DynamicShadow_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;

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

    // 스키닝된 모델 공간의 좌표/노말 계산
    float4 SkinPosition;
    SkinPosition.x = dot(SkinMatrix[0], _Input.Position); // row0 · [x y z 1]
    SkinPosition.y = dot(SkinMatrix[1], _Input.Position); // row1 · [x y z 1]
    SkinPosition.z = dot(SkinMatrix[2], _Input.Position); // row2 · [x y z 1]
    SkinPosition.w = dot(SkinMatrix[3], _Input.Position); // row3 · [x y z 1]
 
    float3 SkinNormal;
    {
        float3 row0 = float3(SkinMatrix[0][0], SkinMatrix[0][1], SkinMatrix[0][2]);
        float3 row1 = float3(SkinMatrix[1][0], SkinMatrix[1][1], SkinMatrix[1][2]);
        float3 row2 = float3(SkinMatrix[2][0], SkinMatrix[2][1], SkinMatrix[2][2]);

        SkinNormal.x = dot(row0, _Input.Normal.xyz);
        SkinNormal.y = dot(row1, _Input.Normal.xyz);
        SkinNormal.z = dot(row2, _Input.Normal.xyz);
        SkinNormal = normalize(SkinNormal);
    }
    
    // Position 설정
    float4 WorldPos = mul(SkinPosition, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    OutPut.WVPosition = ViewPos;
    OutPut.WVPosition.w = 1.0f;
    
    OutPut.Position = mul(SkinPosition, WorldViewProjectionMatrix);
    OutPut.WVPPosition = OutPut.Position;
    OutPut.WVPPosition.w = 1.0f;
    
    return OutPut;
}

//OutPut.Position = SkinPosition;
//OutPut.WVPosition = mul(SkinPosition, WorldViewMatrix);
//OutPut.WVPosition.w = 1.0f;
//OutPut.WVPPosition = mul(SkinPosition, WorldViewProjectionMatrix);
//OutPut.WVPPosition.w = 1.0f;