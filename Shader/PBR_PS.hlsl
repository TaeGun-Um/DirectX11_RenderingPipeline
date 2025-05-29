Texture2D BaseColorTex : register(t0);
Texture2D NormalTex : register(t1);
Texture2D RoughnessTex : register(t2);
Texture2D MetallicTex : register(t3);
Texture2D EmissiveTex : register(t4);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

float4 PBR_PS(PSInput _input) : SV_TARGET
{
    float4 BaseColor = BaseColorTex.Sample(Sampler, _input.TexCoord);
    float3 Normal = NormalTex.Sample(Sampler, _input.TexCoord).xyz;
    float Roughness = RoughnessTex.Sample(Sampler, _input.TexCoord).r;
    float Metallic = MetallicTex.Sample(Sampler, _input.TexCoord).r;
    float3 Emissive = EmissiveTex.Sample(Sampler, _input.TexCoord).rgb;

    
    // ? ������ ����ϴ� ��� (��: ���� ä�ο� ����), ����ȭ������ ���÷��ǿ��� ȣ���� ����
    float Temp = Normal.x + Roughness + Metallic;
    float Alpha = saturate(Temp * 0.001f + 1.0f); // ������ ���� ����

    float3 FinalColor = BaseColor.rgb + Emissive;
    return float4(FinalColor, Alpha);
}