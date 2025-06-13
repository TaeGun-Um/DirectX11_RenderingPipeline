cbuffer OldFilmData : register(b0)
{
    float4 OldFilmValue; // x ���и� ���
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
    // 1) ��ũ�� ���(Film) �ؽ�ó ����
    float4 FlimColor = DiffuseTex.Sample(Sampler, IN.Texcoord);

    // 2) ��� ����ũ ���� ��� (1?R) * OldFilmValue.x
    float Mask = (1.0f - FlimColor.r) * OldFilmValue.x;

    // 3) R, G, B, A ��� ������ ����ũ ������
    return float4(Mask, Mask, Mask, Mask);
}