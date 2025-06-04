Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

float4 Basic_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    return Color;
}

