//#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION0;
    float3 WorldViewPosition : POSITION1;
    float3 WorldNormal : NORMAL0;
    float3 WorldViewNormal : NORMAL1;
};

struct PSOutPut
{
    float4 MeshTarget : SV_Target0;
    float4 PositionTarget : SV_Target1;
    float4 NormalTarget : SV_Target2;
};

 // 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
 // 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
PSOutPut Grapics_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut Output;
    
    // 텍스처 색상
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    Output.MeshTarget = Albedo;
    Output.PositionTarget = float4(_Input.WorldViewPosition.xyz, 1.0f);
    Output.NormalTarget = float4(_Input.WorldViewNormal, 0.1f);

    return Output;
}


// 원본
//struct PSInput
//{
//    float4 Position : SV_POSITION;
//    float2 TexCoord : TEXCOORD;
//    float3 WorldPosition : POSITION0;
//    float3 ViewPosition : POSITION1;
//    float3 WorldNormal : NORMAL0;
//    float3 WorldViewNormal : NORMAL1;
//};

//float4 Grapics_PS(PSInput _Input) : SV_TARGET
//{
//    //PSOutPut Output;
 
//    // 텍스처 색상
//    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord.xy);
 
//    // 월드공간 기준 픽셀(표면) 위치와 법선 단위 벡터
//    float3 PixelPosition = _Input.WorldPosition;
//    float3 PixelNormal = normalize(_Input.WorldNormal);
 
//    // 누적 조명 컬러 (RGB)
//    float3 AccumLightColor = float3(0, 0, 0);
 
//    for (int i = 0; i < LightCount; ++i)
//    {
//        LightData LTData = Lights[i];
     
//        if (LTData.bIsLightSet == false)
//        {
//            continue;
//        }
     
//        float Shininess = 32.0f; // Shininess는 임의로 32 고정
     
//        if (LTData.LightType == 0) // Directional Light
//        {
//            float3 DiffuseLight = DiffuseLightCalculation(-LTData.LightForwardVector.xyz, _Input.WorldNormal); // Diffuse Light 계산
//            float3 SpecularLight = SpecularLightCalculation(-LTData.LightForwardVector.xyz, _Input.WorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
//            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light 계산, 강도는 LightColor의 w값 사용
     
//            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
//        }
//        else if (LTData.LightType == 1) // 포인트 라이트
//        {
//            // 픽셀과 위치와 광원 위치 사이 거리 및 방향 구하기
//            float3 LightPosition = LTData.LightWorldPosition.xyz;
//            float3 PixelToLight = LightPosition - PixelPosition;
//            float Distance = length(PixelToLight);
         
//            float3 DiffuseLight = DiffuseLightCalculation(PixelToLight, _Input.WorldNormal); // Diffuse Light 계산
//            float3 SpecularLight = SpecularLightCalculation(PixelToLight, _Input.WorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
//            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light 계산, 강도는 LightColor의 w값 사용
         
//            // 감쇠(Attenuation) 공식 = 1/(c0 + c1·d + c2·d²)
//            float C0 = 1.0f;
//            float C1 = 0.0f;
//            float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // 거리에 따른 제곱항 계수, 여기를 조정하면 감쇠가 쌔짐
//            float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);
         
//            // 누적
//            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight) * Attenuation;
//        }
//    }
 
//    Albedo.rgb *= AccumLightColor;
 
//    return Albedo;
//}