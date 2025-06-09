#include "LightData.fx"

struct PSInput
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
};

struct PSOutPut
{
    float4 DiffuseTarget : SV_Target0;
    float4 SpecularTarget : SV_Target1;
    float4 AmbientTarget : SV_Target2;
    float4 ShadowTarget : SV_Target3;
};

Texture2D PositionTex : register(t0); // G-Buffer: 뷰-스페이스 위치(x,y,z) + 1
Texture2D NormalTex : register(t1); // G-Buffer: 뷰-스페이스 법선(x,y,z) + 1
Texture2D ShadowTex : register(t2);
SamplerState PointWrap : register(s0);

PSOutPut DeferredLight_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut OutPut;
        
    float3 WorldViewPos = PositionTex.Sample(PointWrap, _Input.Texcoord).xyz;
    float3 WorldViewNorm = NormalTex.Sample(PointWrap, _Input.Texcoord).xyz;
        
    LightData LTData = Lights[LightCount];
    
    float4 WorldViewPos4 = float4(WorldViewPos, 1.0f);
    float3 WorldPos = mul(WorldViewPos4, LTData.CameraViewInverseMatrix).xyz;
    float3 WorldNorm = normalize(mul(WorldViewNorm, (float3x3)LTData.CameraViewInverseMatrix));
    
    float3 LightDirection = normalize(LTData.LightType == 0 ? -LTData.LightForwardVector.xyz : (LTData.LightWorldPosition.xyz - WorldPos));
    float3 CameraWorldPos = LTData.CameraWorldPosition.xyz;
    
    float3 DiffuseLight;
    float3 SpecularLight;
    float3 AmbientLight;
    float Attenuation = 1.0f;
    float Shininess = 32.0f; // Shininess는 임의로 32 고정
     
    DiffuseLight = DiffuseLightCalculation(LightDirection, WorldNorm);
    SpecularLight = SpecularLightCalculation(LightDirection, WorldNorm, CameraWorldPos, WorldNorm, Shininess);
    AmbientLight = AmbientLightCalculation(LTData.LightColor.w);
    
    if (LTData.LightType == 1) // 포인트 라이트
    {
        // 픽셀과 위치와 광원 위치 사이 거리 및 방향 구하기
        float Distance = length(LightDirection);
  
        // 감쇠(Attenuation) 공식 = 1/(c0 + c1·d + c2·d²)
        float C0 = 1.0f;
        float C1 = 0.0f;
        float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // 거리에 따른 제곱항 계수, 여기를 조정하면 감쇠가 쌔짐
        Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);
    }
    
    //// DiffuseLight가 비춰지는 곳이라면
    //if (DiffuseLight.x > 0.0f)
    //{
    //    // 빛이존재하므로 그림자도 존재해야할지 판단(빛을 기준으로한 포지션으로 변경)
    //    float4 WVPPixelPos = mul(WorldPos, LTData.LightViewProjectionMatrix);
        
    //    // worldviewprojection 이 곱해지면 그건 -1~1사이의 공간입니까? w에 곱해지기전의 z값을 보관해 놓은 값이 됩니다. // 모든 값은 -1~1사이의 값이 됩니다.
    //    float3 PixelProjection = WVPPixelPos.xyz / WVPPixelPos.w;
        
    //    float2 ShadowUV = float2(PixelProjection.x * 0.5f + 0.5f, PixelProjection.y * -0.5f + 0.5f);
    //    float ShadowDepth = ShadowTex.Sample(PointWrap, float2(ShadowUV.x, ShadowUV.y)).r;
        
    //    // 가장 외각을 약간 깎아내서 
    //    if (
    //        ShadowDepth >= 0.0f
    //        && 0.001f < ShadowUV.x && 0.999f > ShadowUV.x
    //        && 0.001f < ShadowUV.y && 0.999f > ShadowUV.y
    //        && PixelProjection.z >= (ShadowDepth + 0.001f)
    //        )
    //    {
    //        OutPut.ShadowTarget.x = 1.0f;
    //        OutPut.ShadowTarget.a = 1.0f;
    //    }
    //    else
    //    {
    //        // 그림자 바깥(Light)  → x 채널에 0을 넣어 “그림자 아님” 표시, a 채널은 그대로 1로 유지
    //        OutPut.ShadowTarget.x = 0.0f;
    //        OutPut.ShadowTarget.a = 1.0f;
    //    }
    //}
    
    DiffuseLight *= Attenuation;
    SpecularLight *= Attenuation;
    AmbientLight *= Attenuation;
    
    OutPut.DiffuseTarget = float4(DiffuseLight, 1.0f);
    OutPut.SpecularTarget = float4(SpecularLight, 1.0f);
    OutPut.AmbientTarget = float4(AmbientLight, 1.0f);
    
    float Shadow = ShadowTex.Sample(PointWrap, _Input.Texcoord).r;
    OutPut.ShadowTarget = float4(Shadow, Shadow, Shadow, 1.0f);
    
    return OutPut; // 카메라 행렬 빛의 위치 그려져있는 빛을 기반으로한 깊이 버퍼 텍스처 리턴
}