#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

// 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
// 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
float4 FGraphics_PS(PSInput _Input) : SV_TARGET
{
    // 텍스처 색상
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    // 월드공간 기준 픽셀(표면) 위치와 법선 단위 벡터
    float3 PixelPosition = _Input.WorldPosition;
    float3 PixelNormal = normalize(_Input.WorldNormal);
    
    // 누적 조명 컬러 (RGB)
    float3 AccumLightColor = float3(0, 0, 0);
    
    for (int i = 0; i < LightCount; ++i)
    {
        LightData LTData = Lights[i];
        
        if (LTData.bIsLightSet == false)
        {
            continue;
        }
        
        // 공통분모
        float3 FinalLight = float3(0.0f, 0.0f, 0.0f);
        float3 LightDirection = normalize(-LTData.LightForwardVector.xyz); // 빛 계산에 사용하는 LightDirection은 "표면에서 봤을 때 빛이 표면으로 들어오는 방향"을 사용하므로 반대로 뒤집음
        
        float Shininess = 32.0f; // Shininess는 임의로 32 고정
        float AmbientIntensity = LTData.LightColor.w;
        
        // 월드공간 기준 시선 위치와 시선의 방향 단위 벡터
        float3 EyePosition = LTData.CameraWorldPosition.xyz;
        float3 EyeDirection = normalize(EyePosition - _Input.WorldPosition);
        
        if (LTData.LightType == 0) // Directional Light
        {
            // Diffuse Light 계산
            float DiffuseLight = saturate(dot(LightDirection, PixelNormal));
        
            // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
            float3 ReflectionVector = normalize(2.0f * PixelNormal * dot(LightDirection, PixelNormal) - LightDirection); // 반사벡터
            float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
            float3 SpecularLight = pow(RDotV, Shininess);
        
            // Ambient Light 계산, 강도는 LightColor의 w값 사용
            float3 AmbientLight = AmbientIntensity;
        
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
        }
        else if (LTData.LightType == 1) // 포인트 라이트
        {
            float3 LightPosition = LTData.LightWorldPosition.xyz;
        
            // 픽셀과 위치와 광원 위치 사이 거리 및 방향 구하기
            float3 PixelToLight = LightPosition - PixelPosition;
            float Distance = length(PixelToLight); // Distance로 Near, Far 컬링하면 재밌는거 볼 수 있음, 밑에 주석을 사용
            //if (Distance >= LTData.NearDistance && Distance <= LTData.FarDistance)
            
            // 픽셀에서 광원으로 향하는 단위 벡터
            float3 Vector = normalize(PixelToLight);
            
            // 감쇠(Attenuation) 공식 = 1/(c0 + c1·d + c2·d²)
            float C0 = 1.0f;
            float C1 = 0.0f;
            float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // 거리에 따른 제곱항 계수, 여기를 조정하면 감쇠가 쌔짐
            float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);

            // Diffuse Light 계산
            float DiffuseLight = saturate(dot(PixelNormal, Vector));

            // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
            float3 ReflectionVector = normalize(2.0f * PixelNormal * dot(Vector, PixelNormal) - Vector);
            float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
            float3 SpecularLight = pow(RDotV, Shininess);

            // Ambient Light 계산, 강도는 LightColor의 w값 사용
            float3 AmbientLight = AmbientIntensity;

            // 누적
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight) * Attenuation;
        }
    }
    
    Albedo.rgb *= AccumLightColor;
    return Albedo;
}