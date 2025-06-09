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
SamplerState PointWrap : register(s0);

PSOutput DeferredMerge_PS(PSInput _Input) : SV_TARGET
{
    PSOutput OutPut;
    
    float4 Albedo = BaseColorTex.Sample(PointWrap, _Input.TEXCOORD.xy);
    float4 DiffuseRatio = DiffuseTex.Sample(PointWrap, _Input.TEXCOORD.xy);
    float4 SpacularRatio = SpecularTex.Sample(PointWrap, _Input.TEXCOORD.xy);
    float4 AmbientRatio = AmbientTex.Sample(PointWrap, _Input.TEXCOORD.xy);
    
    //if (�� �տ� ��ü�� �ִٸ�)
    //{
    //     DiffuseRatio.xyz *= 0.01f;
    //     SpacularRatio.xyz *= 0.01f;
    //     AmbientRatio.xyz *= 0.01f;
    //    ���� ���� �������� �Ѵ�.
    //}
        
    if (Albedo.a)
    {
        // 0.1f
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

    //NewOutPut.DifLight = float4(1.0f, 0.0f, 0.0f, 1.0f);
    //NewOutPut.SpcLight = float4(0.0f, 1.0f, 0.0f, 1.0f);
    //NewOutPut.AmbLight = float4(0.0f, 0.0f, 1.0f, 1.0f);
    
    // return NewOutPut;
}