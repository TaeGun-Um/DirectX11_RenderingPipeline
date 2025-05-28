Texture2D Texture : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

float4 Basic_PS(PSInput _Input) : SV_TARGET
{
    return Texture.Sample(Sampler, _Input.TexCoord);
}