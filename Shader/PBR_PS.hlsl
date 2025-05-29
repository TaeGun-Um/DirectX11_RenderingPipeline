Texture2D BaseColorTex : register(t0);
Texture2D NormalTex : register(t1);
Texture2D RoughnessTex : register(t2);
Texture2D MetallicTex : register(t3);

SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

float4 PBR_PS(PSInput _input) : SV_TARGET
{
    float3 FinalColor = BaseColorTex.Sample(Sampler, _input.TexCoord).rgb;
    return float4(FinalColor, 1.0f); // Alpha 무조건 고정
}
