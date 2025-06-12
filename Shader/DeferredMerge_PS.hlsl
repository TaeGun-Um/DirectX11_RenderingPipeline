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

Texture2D BaseColorTex : register(t0);
Texture2D DiffuseTex : register(t1);
Texture2D SpecularTex : register(t2);
Texture2D AmbientTex : register(t3);
Texture2D ShadowTex : register(t4);
SamplerState Sampler : register(s0);

PSOutput DeferredMerge_PS(PSInput _Input) : SV_TARGET
{
    PSOutput OutPut = (PSOutput) 0;
    
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TEXCOORD);
    float4 DiffuseRatio = DiffuseTex.Sample(Sampler, _Input.TEXCOORD);
    float4 SpacularRatio = SpecularTex.Sample(Sampler, _Input.TEXCOORD);
    float4 AmbientRatio = AmbientTex.Sample(Sampler, _Input.TEXCOORD);
    float ShadowMask = ShadowTex.Sample(Sampler, _Input.TEXCOORD).x;
    
    // 그림자 어두운 정도 (0.0 = 안 어두움, 1.0 = 완전 검정)
    float ShadowStrength = 0.7;

    // 그림자 계수 계산: 
    // shadowM==0 → 1.0 (원래 라이팅),  
    // shadowM==1 → 1.0-shadowStrength (어두워진 라이팅)
    float ShadowFactor = lerp(1.0, 1.0 - ShadowStrength, ShadowMask);
    
    DiffuseRatio *= ShadowFactor;
    SpacularRatio *= ShadowFactor;
    
    if (Albedo.a)
    {
        OutPut.Color.xyz = Albedo.xyz * (DiffuseRatio.xyz + SpacularRatio.xyz + AmbientRatio.xyz);
        OutPut.Color.a = saturate(Albedo.a + (DiffuseRatio.w + SpacularRatio.w + AmbientRatio.w));
        OutPut.Color.a = 1.0f;
    }
    else
    {
        OutPut.Color.xyz = (DiffuseRatio.xyz + SpacularRatio.xyz + AmbientRatio.xyz);
        OutPut.Color.a = saturate(OutPut.Color.x);
    }
    
    return OutPut;
}