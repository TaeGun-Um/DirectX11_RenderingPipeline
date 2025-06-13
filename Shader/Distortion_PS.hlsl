cbuffer FrameData : register(b0)
{
    float4 ScreenSize; // 화면 크기
    float AccTime; // 실행된 시간
};

Texture2D DiffuseTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION; // 스크린 위치 (필요 없으면 생략 가능)
    float2 Texcoord : TEXCOORD0; // 0~1 범위 UV
};

float4 Distortion_PS(PSInput _Input) : SV_TARGET
{
    float2 UV = _Input.Texcoord;

    // 파라미터, 상수버퍼로 전달하면 조절 가능
    const float Frequency = 10.0f; // 파동 수
    const float Ample = 20.0f; // 진폭 분모

    // 공식
    UV.x += sin(UV.y * Frequency + AccTime) / Ample;

    return DiffuseTex.Sample(Sampler, UV);
}

//-----------------------------------------------------------------------------
// 테크닉 / 패스 (Effect Framework 사용 시)
//-----------------------------------------------------------------------------
//technique11 PostProcessTech
//{
//    pass P0
//    {
//        SetVertexShader( CompileShader( vs_5_0, FullScreenVS() ) );
//        SetPixelShader( CompileShader( ps_5_0, PostProcess_PS() ) );
//    }
//}
