cbuffer LightData : register(b0)
{
    float4 LightColor; // RGB(색), w(강도)
	
    float4 LightWorldPosition; // 라이트 현재 위치(월드)
    float4 LightForward; // 라이트 전방 방향

    float4 LightViewSpacePosition; // 라이트 뷰스페이스
    float4 LightViewSpaceForward; // 라이트 뷰스페이스 전방 방향
    float4 LightViewSpaceBack; // 라이트 뷰스페이스 후방 빙향

    float LightNear = 1.0f;
    float LightFar = 1.0f;
    int LightType = 0;
}

Texture2D BaseColorTex : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

float4 Grapics_PS(PSInput _Input) : SV_TARGET
{
    // 텍스처 색상
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);

    // 정규화된 노멀 및 라이트 방향
    float3 Normal = normalize(_Input.Normal.xyz);
    float3 LightDir = normalize(-LightForward.xyz); // 역방향 (빛이 오는 방향)

    // Lambert 조명 모델 (디퓨즈)
    float Diff = max(dot(Normal, LightDir), 0.0f);

    // 조명 색 * 확산 * 텍스처 색
    float3 LitColor = Albedo.rgb * Diff * LightColor.rgb;

    return float4(LitColor, Albedo.a);
}