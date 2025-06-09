#include "LightData.fx"

Texture2D BaseColorTex : register(t0);
Texture2D NormalTex : register(t1);
//Texture2D ShadowTex : register(t2);

// Texture2D RoughnessTex : register(t2);
// Texture2D SpecularTex : register(t3);
// Texture2D AmbientTex : register(t3);
// Texture2D EmissiveTex : register(t3);
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

// 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
// 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
float4 PBR_PS(PSInput _Input) : SV_TARGET
{
    // 텍스처 색상
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord.xy);
    
    // 월드공간 기준 픽셀(표면) 위치와 법선 단위 벡터
    float3 PixelPosition = _Input.WorldPosition;
    
    // 노말 맵 샘플링 (Tangent-space 노말 → [-1, 1] 범위)
    float3 SampledNormalTS = NormalTex.Sample(Sampler, _Input.TexCoord.xy).xyz * 2.0f - 1.0f;
    SampledNormalTS = normalize(SampledNormalTS);
    
    // TBN 매트릭스 구성 (열(column) 단위로 저장)
    float3x3 TBNMatrix;
    TBNMatrix[0] = normalize(_Input.WorldTangent); // T
    TBNMatrix[1] = normalize(_Input.WorldBinormal); // B
    TBNMatrix[2] = normalize(_Input.WorldNormal); // N
    
    float3 MappedWorldNormal = normalize(mul(SampledNormalTS, TBNMatrix));
    
    // 누적 조명 컬러 (RGB)
    float3 AccumLightColor = float3(0, 0, 0);
    
    for (int i = 0; i < LightCount; ++i)
    {
        LightData LTData = Lights[i];
        
        if (LTData.bIsLightSet == false)
        {
            continue;
        }
        
        float Shininess = 32.0f; // Shininess는 임의로 32 고정
        
        if (LTData.LightType == 0) // Directional Light
        {
            float3 DiffuseLight = DiffuseLightCalculation(-LTData.LightForwardVector.xyz, MappedWorldNormal); // Diffuse Light 계산
            float3 SpecularLight = SpecularLightCalculation(-LTData.LightForwardVector.xyz, MappedWorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light 계산, 강도는 LightColor의 w값 사용
        
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
        }
        else if (LTData.LightType == 1) // 포인트 라이트
        {
            // 픽셀과 위치와 광원 위치 사이 거리 및 방향 구하기
            float3 LightPosition = LTData.LightWorldPosition.xyz;
            float3 PixelToLight = LightPosition - PixelPosition;
            float Distance = length(PixelToLight);
            
            float3 DiffuseLight = DiffuseLightCalculation(PixelToLight, MappedWorldNormal); // Diffuse Light 계산
            float3 SpecularLight = SpecularLightCalculation(PixelToLight, MappedWorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light 계산, 강도는 LightColor의 w값 사용
            
            // 감쇠(Attenuation) 공식 = 1/(c0 + c1·d + c2·d²)
            float C0 = 1.0f;
            float C1 = 0.0f;
            float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // 거리에 따른 제곱항 계수, 여기를 조정하면 감쇠가 쌔짐
            float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);
            
            // 누적
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight) * Attenuation;
        }
    }
    
    Albedo.rgb *= AccumLightColor;
    return Albedo;
}