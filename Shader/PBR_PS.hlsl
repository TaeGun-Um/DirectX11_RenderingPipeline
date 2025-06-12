#include "LightData.fx"

Texture2D BaseColorTex : register(t0);
Texture2D NormalTex : register(t1);
SamplerState Sampler : register(s0); // 샘플러

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

// 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
// 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
PSOutPut PBR_PS(PSInput _Input) : SV_TARGET
{
    //// 텍스처 색상
    //float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord.xy);
    
    //// 월드공간 기준 픽셀(표면) 위치와 법선 단위 벡터
    //float3 PixelPosition = _Input.WorldPosition;
    
    //// 노말 맵 샘플링 (Tangent-space 노말 → [-1, 1] 범위)
    //float3 SampledNormalTS = NormalTex.Sample(Sampler, _Input.TexCoord.xy).xyz * 2.0f - 1.0f;
    //SampledNormalTS = normalize(SampledNormalTS);
    
    //// TBN 매트릭스 구성 (열(column) 단위로 저장)
    //float3x3 TBNMatrix;
    //TBNMatrix[0] = normalize(_Input.WorldTangent); // T
    //TBNMatrix[1] = normalize(_Input.WorldBinormal); // B
    //TBNMatrix[2] = normalize(_Input.WorldNormal); // N
    
    //float3 MappedWorldNormal = normalize(mul(SampledNormalTS, TBNMatrix));
    
    PSOutPut Output = (PSOutPut) 0;
    
    Output.MeshTarget = BaseColorTex.Sample(Sampler, _Input.TexCoord); // 텍스쳐컬러
    Output.PositionTarget = float4(_Input.WorldPosition.xyz, 1.0f); // 월드스페이스 Position
    Output.NormalTarget = float4(_Input.WorldNormal, 1.0f); // 월드스페이스 Normal
    Output.TangentTarget = float4(_Input.WorldTangent, 1.0f); // 월드스페이스 Normal
    Output.BinormalTarget = float4(_Input.WorldBinormal, 1.0f); // 월드스페이스 Normal

    return Output;
}