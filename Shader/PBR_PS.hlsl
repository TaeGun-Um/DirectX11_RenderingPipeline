#include "LightData.fx"

Texture2D BaseColorTex : register(t0);
Texture2D NormalTex : register(t1);
//Texture2D ShadowTex : register(t2);

// Texture2D RoughnessTex : register(t2);
// Texture2D SpecularTex : register(t3);
// Texture2D AmbientTex : register(t3);
// Texture2D EmissiveTex : register(t3);
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

// �� ���Ϳ� normalize�� ���ִ� ������, ��������� normalize�� ���͸� �־���� �ϴ��� 
// ������ ���� ��� ������ �𸣱� ������ �׳� ���ִ°�(�������� ���� ó����� ���� ��)
float4 PBR_PS(PSInput _Input) : SV_TARGET
{
    // �ؽ�ó ����
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord.xy);
    
    // ������� ���� �ȼ�(ǥ��) ��ġ�� ���� ���� ����
    float3 PixelPosition = _Input.WorldPosition;
    
    // �븻 �� ���ø� (Tangent-space �븻 �� [-1, 1] ����)
    float3 SampledNormalTS = NormalTex.Sample(Sampler, _Input.TexCoord.xy).xyz * 2.0f - 1.0f;
    SampledNormalTS = normalize(SampledNormalTS);
    
    // TBN ��Ʈ���� ���� (��(column) ������ ����)
    float3x3 TBNMatrix;
    TBNMatrix[0] = normalize(_Input.WorldTangent); // T
    TBNMatrix[1] = normalize(_Input.WorldBinormal); // B
    TBNMatrix[2] = normalize(_Input.WorldNormal); // N
    
    float3 MappedWorldNormal = normalize(mul(SampledNormalTS, TBNMatrix));
    
    // ���� ���� �÷� (RGB)
    float3 AccumLightColor = float3(0, 0, 0);
    
    for (int i = 0; i < LightCount; ++i)
    {
        LightData LTData = Lights[i];
        
        if (LTData.bIsLightSet == false)
        {
            continue;
        }
        
        float Shininess = 32.0f; // Shininess�� ���Ƿ� 32 ����
        
        if (LTData.LightType == 0) // Directional Light
        {
            float3 DiffuseLight = DiffuseLightCalculation(-LTData.LightForwardVector.xyz, MappedWorldNormal); // Diffuse Light ���
            float3 SpecularLight = SpecularLightCalculation(-LTData.LightForwardVector.xyz, MappedWorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light ���, ������ LightColor�� w�� ���
        
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
        }
        else if (LTData.LightType == 1) // ����Ʈ ����Ʈ
        {
            // �ȼ��� ��ġ�� ���� ��ġ ���� �Ÿ� �� ���� ���ϱ�
            float3 LightPosition = LTData.LightWorldPosition.xyz;
            float3 PixelToLight = LightPosition - PixelPosition;
            float Distance = length(PixelToLight);
            
            float3 DiffuseLight = DiffuseLightCalculation(PixelToLight, MappedWorldNormal); // Diffuse Light ���
            float3 SpecularLight = SpecularLightCalculation(PixelToLight, MappedWorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light ���, ������ LightColor�� w�� ���
            
            // ����(Attenuation) ���� = 1/(c0 + c1��d + c2��d��)
            float C0 = 1.0f;
            float C1 = 0.0f;
            float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // �Ÿ��� ���� ������ ���, ���⸦ �����ϸ� ���谡 ����
            float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);
            
            // ����
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight) * Attenuation;
        }
    }
    
    Albedo.rgb *= AccumLightColor;
    return Albedo;
}