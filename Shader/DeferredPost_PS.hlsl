struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

struct LightOutPut
{
    float4 DiffuseTarget : SV_TARGET0;
    float4 SpecularTarget : SV_TARGET1;
    float4 AmbientTarget : SV_TARGET2;
};

Texture2D DiffuseTex : register(t0);
Texture2D SpecularTex : register(t1);
Texture2D AmbientTex : register(t2);
Texture2D ShadowTex : register(t3);
SamplerState Sampler : register(s0);

LightOutPut DeferredPost_PS(PSInput _Input) : SV_TARGET
{
    LightOutPut OutPut;
    
    OutPut.DiffuseTarget = DiffuseTex.Sample(Sampler, _Input.Texcoord);
    OutPut.SpecularTarget = SpecularTex.Sample(Sampler, _Input.Texcoord);
    OutPut.AmbientTarget = AmbientTex.Sample(Sampler, _Input.Texcoord);
    float4 Shadow = ShadowTex.Sample(Sampler, _Input.Texcoord);
    
    // 실제 그림자 로직
    if (Shadow.x != 0.0f)
    {
        OutPut.DiffuseTarget = 0.2f / Shadow.x;
        OutPut.SpecularTarget = 0.2f / Shadow.x;
    }
    
    return OutPut;
}