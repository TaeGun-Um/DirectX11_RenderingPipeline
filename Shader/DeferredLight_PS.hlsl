#include "LightData.fx"

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

struct PSOutPut
{
    float4 DiffuseTarget : SV_TARGET0;
    float4 SpecularTarget : SV_TARGET1;
    float4 AmbientTarget : SV_TARGET2;
    float4 ShadowTarget : SV_TARGET3;
};

Texture2D PositionTex : register(t0); // G-Buffer: 월드-스페이스 위치(x,y,z) + 1
Texture2D NormalTex : register(t1); // G-Buffer: 월드-스페이스 법선(x,y,z) + 1
Texture2D ShadowTex : register(t2);
SamplerState Sampler : register(s0);
SamplerComparisonState ShadowCompare : register(s1);

PSOutPut DeferredLight_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut OutPut = (PSOutPut) 0;
        
    float3 WorldPos = PositionTex.Sample(Sampler, _Input.Texcoord).xyz;
    float3 WorldNorm = NormalTex.Sample(Sampler, _Input.Texcoord).xyz;
        
    LightData LTData = Lights[LightCount];
   
    // 공통 파라미터
    float Shininess = 32.0f;
    float3 EyePosition = LTData.CameraWorldPosition.xyz;
    float3 EyeDirection; // 계산 시점에 맞춰 설정
    
    float3 DiffuseLight = float3(0.0f, 0.0f, 0.0f);
    float3 SpecularLight = float3(0.0f, 0.0f, 0.0f);
    float3 AmbientLight = float3(0.0f, 0.0f, 0.0f);
    
    // Directional Light 분기
    if (LTData.LightType == 0)
    {
        // 라이트 방향: surface → light 이므로 LightForwardVector에 -1을 곱함
        float3 LightDirection = -LTData.LightForwardVector.xyz;

        // Diffuse / Specular / Ambient
        DiffuseLight = DiffuseLightCalculation(LightDirection, WorldNorm);
        SpecularLight = SpecularLightCalculation(LightDirection, WorldNorm, EyePosition, WorldPos, Shininess);
        AmbientLight = AmbientLightCalculation(LTData.LightColor.w);
        
        DiffuseLight *= LTData.LightColor.xyz;
        SpecularLight *= LTData.LightColor.xyz;
        AmbientLight *= LTData.LightColor.xyz;
    }
    else if (LTData.LightType == 1)  // Point Light 분기
    {
        // 픽셀 → 광원 벡터
        float3 PixelToLight = LTData.LightWorldPosition.xyz - WorldPos;
        float Distance = length(PixelToLight);
        float3 Vector = normalize(PixelToLight);

        // Diffuse / Specular / Ambient
        DiffuseLight = DiffuseLightCalculation(Vector, WorldNorm);
        SpecularLight = SpecularLightCalculation(Vector, WorldNorm, EyePosition, WorldPos, Shininess);
        AmbientLight = AmbientLightCalculation(LTData.LightColor.w);

        // 거리 기반 감쇠
        float C0 = 1.0f;
        float C1 = 0.0f;
        float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance);
        float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);
        
        DiffuseLight *= Attenuation;
        SpecularLight *= Attenuation;
        AmbientLight *= Attenuation;
        
        DiffuseLight *= LTData.LightColor.xyz;
        SpecularLight *= LTData.LightColor.xyz;
        AmbientLight *= LTData.LightColor.xyz;
    }

    if (DiffuseLight.x > 0.0f)
    {
        float4 LightPos = mul(float4(WorldPos.xyz, 1.0f), LTData.LightViewProjectionMatrix);
        
        // worldviewprojection 이 곱해지면 그건 -1~1사이의 공간입니까? w에 곱해지기전의 z값을 보관해 놓은 값이 됩니다. // 모든 값은 -1~1사이의 값이 됩니다.
        float3 LightProjection = LightPos.xyz / LightPos.w;
        
        float2 ShadowUV = float2(LightProjection.x * 0.5f + 0.5f, LightProjection.y * -0.5f + 0.5f);
        float fShadowDepth = ShadowTex.Sample(Sampler, float2(ShadowUV.x, ShadowUV.y)).r;
        
       
        // 가장 외각을 약간 깎아내서 
        if (
            fShadowDepth >= 0.0f
            && 0.001f < ShadowUV.x && 0.999f > ShadowUV.x
            && 0.001f < ShadowUV.y && 0.999f > ShadowUV.y
            && LightProjection.z >= (fShadowDepth + 0.001f)
            )
        {
            OutPut.ShadowTarget.x = 1.0f;
            OutPut.ShadowTarget.a = 1.0f;
        }
        // 그림자가 드리우는 부분 마스킹, x니까 빨간색으로 마스킹됨
    }
    
    OutPut.DiffuseTarget = float4(DiffuseLight, 1.0f);
    OutPut.SpecularTarget = float4(SpecularLight, 1.0f);
    OutPut.AmbientTarget = float4(AmbientLight, 1.0f);
    
    return OutPut; // 카메라 행렬 빛의 위치 그려져있는 빛을 기반으로한 깊이 버퍼 텍스처 리턴
}