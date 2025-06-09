Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

// �ؽ��ĸ� �ް� �׳� �״�� ���
float4 NonG_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    return Color;
}