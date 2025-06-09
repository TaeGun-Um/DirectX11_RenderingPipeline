Texture2D BaseColorTex : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

// 텍스쳐만 받고 그냥 그대로 출력
float4 NonG_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    return Color;
}