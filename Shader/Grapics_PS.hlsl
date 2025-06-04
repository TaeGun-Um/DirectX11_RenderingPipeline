cbuffer LightData : register(b0)
{
    float4 LightColor; // RGB(��), w(����)
	
    float4 LightWorldPosition; // ����Ʈ ���� ��ġ(����)
    float4 LightForward; // ����Ʈ ���� ����

    float4 LightViewSpacePosition; // ����Ʈ �佺���̽�
    float4 LightViewSpaceForward; // ����Ʈ �佺���̽� ���� ����
    float4 LightViewSpaceBack; // ����Ʈ �佺���̽� �Ĺ� ����

    float LightNear = 1.0f;
    float LightFar = 1.0f;
    int LightType = 0;
}

Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

float4 Grapics_PS(PSInput _Input) : SV_TARGET
{
    // �ؽ�ó ����
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);

    // ����ȭ�� ��� �� ����Ʈ ����
    float3 Normal = normalize(_Input.Normal.xyz);
    float3 LightDir = normalize(-LightForward.xyz); // ������ (���� ���� ����)

    // Lambert ���� �� (��ǻ��)
    float Diff = max(dot(Normal, LightDir), 0.0f);

    // ���� �� * Ȯ�� * �ؽ�ó ��
    float3 LitColor = Albedo.rgb * Diff * LightColor.rgb;

    return float4(LitColor, Albedo.a);
}