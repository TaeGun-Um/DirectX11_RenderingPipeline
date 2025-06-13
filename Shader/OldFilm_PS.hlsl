cbuffer OldFilmData : register(b0)
{
    float4 OldFilmValue; // x 성분만 사용
}

Texture2D DiffuseTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD0;
};

float4 OldFilm_PS(PSInput IN) : SV_TARGET
{
    // 1) 스크린 얼룩(Film) 텍스처 샘플
    float4 FlimColor = DiffuseTex.Sample(Sampler, IN.Texcoord);

    // 2) 얼룩 마스크 강도 계산 (1?R) * OldFilmValue.x
    float Mask = (1.0f - FlimColor.r) * OldFilmValue.x;

    // 3) R, G, B, A 모두 동일한 마스크 값으로
    return float4(Mask, Mask, Mask, Mask);
}