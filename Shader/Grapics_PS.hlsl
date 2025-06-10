#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

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

 // 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
 // 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
PSOutPut Grapics_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut Output = (PSOutPut) 0;
    
    Output.MeshTarget = BaseColorTex.Sample(Sampler, _Input.TexCoord); // 텍스쳐컬러
    Output.WPositionTarget = float4(_Input.WorldPosition.xyz, 1.0f); // 월드스페이스 Position
    Output.WVPositionTarget = float4(_Input.WorldViewPosition.xyz, 1.0f); // 뷰스페이스 Position
    Output.WNormalTarget = float4(_Input.WorldNormal, 1.0f); // 월드스페이스 Normal
    Output.WVNormalTarget = float4(_Input.WorldViewNormal, 1.0f); // 뷰스페이스 Normal

    return Output;
}