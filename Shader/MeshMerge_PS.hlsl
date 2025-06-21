#include "LightData.fx"

struct PSInput
{
    float4 POSITION : SV_POSITION;
    float2 TEXCOORD : TEXCOORD;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

Texture2D BaseMeshTex : register(t0);
Texture2D BaseMeshPosTex : register(t1);
Texture2D AlphaMeshTex : register(t2);
Texture2D AlphaMeshPosTex : register(t3);
SamplerState Sampler : register(s0);

PSOutput MeshMerge_PS(PSInput _Input) : SV_TARGET
{
    PSOutput OutPut = (PSOutput) 0;
    
    float4 BaseColor = BaseMeshTex.Sample(Sampler, _Input.TEXCOORD);
    float4 BasePosition = BaseMeshPosTex.Sample(Sampler, _Input.TEXCOORD);
    
    float4 AlphaColor = AlphaMeshTex.Sample(Sampler, _Input.TEXCOORD);
    float4 AlphaPosition = AlphaMeshPosTex.Sample(Sampler, _Input.TEXCOORD);

    float BaseZ = BasePosition.z;
    float AlphaZ = AlphaPosition.z;

    // 깊이값이 작은 쪽이 앞에 있음
    OutPut.Color = (AlphaZ < BaseZ) ? AlphaColor : BaseColor;

    return OutPut;
}