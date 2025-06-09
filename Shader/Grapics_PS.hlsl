#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

// �� ���Ϳ� normalize�� ���ִ� ������, ��������� normalize�� ���͸� �־���� �ϴ��� 
// ������ ���� ��� ������ �𸣱� ������ �׳� ���ִ°�(�������� ���� ó����� ���� ��)
float4 Grapics_PS(PSInput _Input) : SV_TARGET
{
    // �ؽ�ó ����
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord.xy);
    
    // ������� ���� �ȼ�(ǥ��) ��ġ�� ���� ���� ����
    float3 PixelPosition = _Input.WorldPosition;
    float3 PixelNormal = normalize(_Input.WorldNormal);
    
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
            float3 DiffuseLight = DiffuseLightCalculation(-LTData.LightForwardVector.xyz, _Input.WorldNormal); // Diffuse Light ���
            float3 SpecularLight = SpecularLightCalculation(-LTData.LightForwardVector.xyz, _Input.WorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
            float3 AmbientLight = AmbientLightCalculation(LTData.LightColor.w); // Ambient Light ���, ������ LightColor�� w�� ���
        
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
        }
        else if (LTData.LightType == 1) // ����Ʈ ����Ʈ
        {
            // �ȼ��� ��ġ�� ���� ��ġ ���� �Ÿ� �� ���� ���ϱ�
            float3 LightPosition = LTData.LightWorldPosition.xyz;
            float3 PixelToLight = LightPosition - PixelPosition;
            float Distance = length(PixelToLight);
            
            float3 DiffuseLight = DiffuseLightCalculation(PixelToLight, _Input.WorldNormal); // Diffuse Light ���
            float3 SpecularLight = SpecularLightCalculation(PixelToLight, _Input.WorldNormal, LTData.CameraWorldPosition.xyz, _Input.WorldPosition, Shininess); // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
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