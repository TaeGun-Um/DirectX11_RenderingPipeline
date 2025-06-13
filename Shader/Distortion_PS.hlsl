cbuffer FrameData : register(b0)
{
    float4 ScreenSize; // ȭ�� ũ��
    float AccTime; // ����� �ð�
};

Texture2D DiffuseTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION; // ��ũ�� ��ġ (�ʿ� ������ ���� ����)
    float2 Texcoord : TEXCOORD0; // 0~1 ���� UV
};

float4 Distortion_PS(PSInput _Input) : SV_TARGET
{
    float2 UV = _Input.Texcoord;

    // �Ķ����, ������۷� �����ϸ� ���� ����
    const float Frequency = 10.0f; // �ĵ� ��
    const float Ample = 20.0f; // ���� �и�

    // ����
    UV.x += sin(UV.y * Frequency + AccTime) / Ample;

    return DiffuseTex.Sample(Sampler, UV);
}

//-----------------------------------------------------------------------------
// ��ũ�� / �н� (Effect Framework ��� ��)
//-----------------------------------------------------------------------------
//technique11 PostProcessTech
//{
//    pass P0
//    {
//        SetVertexShader( CompileShader( vs_5_0, FullScreenVS() ) );
//        SetPixelShader( CompileShader( ps_5_0, PostProcess_PS() ) );
//    }
//}
