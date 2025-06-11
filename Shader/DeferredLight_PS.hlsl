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

Texture2D PositionTex : register(t0); // G-Buffer: ����-�����̽� ��ġ(x,y,z) + 1
Texture2D NormalTex : register(t1); // G-Buffer: ����-�����̽� ����(x,y,z) + 1
Texture2D ShadowTex : register(t2);
SamplerState Sampler : register(s0);
SamplerComparisonState ShadowCompare : register(s1);

PSOutPut DeferredLight_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut OutPut = (PSOutPut) 0;
        
    float3 WorldPos = PositionTex.Sample(Sampler, _Input.Texcoord).xyz;
    float3 WorldNorm = NormalTex.Sample(Sampler, _Input.Texcoord).xyz;
        
    LightData LTData = Lights[LightCount];
   
    // ���� �Ķ����
    float Shininess = 32.0f;
    float3 EyePosition = LTData.CameraWorldPosition.xyz;
    float3 EyeDirection; // ��� ������ ���� ����
    
    float3 DiffuseLight = float3(0.0f, 0.0f, 0.0f);
    float3 SpecularLight = float3(0.0f, 0.0f, 0.0f);
    float3 AmbientLight = float3(0.0f, 0.0f, 0.0f);
    
    // Directional Light �б�
    if (LTData.LightType == 0)
    {
        // ����Ʈ ����: surface �� light �̹Ƿ� LightForwardVector�� -1�� ����
        float3 LightDirection = -LTData.LightForwardVector.xyz;

        // Diffuse / Specular / Ambient
        DiffuseLight = DiffuseLightCalculation(LightDirection, WorldNorm);
        SpecularLight = SpecularLightCalculation(LightDirection, WorldNorm, EyePosition, WorldPos, Shininess);
        AmbientLight = AmbientLightCalculation(LTData.LightColor.w);
        
        DiffuseLight *= LTData.LightColor.xyz;
        SpecularLight *= LTData.LightColor.xyz;
        AmbientLight *= LTData.LightColor.xyz;
    }
    else if (LTData.LightType == 1)  // Point Light �б�
    {
        // �ȼ� �� ���� ����
        float3 PixelToLight = LTData.LightWorldPosition.xyz - WorldPos;
        float Distance = length(PixelToLight);
        float3 Vector = normalize(PixelToLight);

        // Diffuse / Specular / Ambient
        DiffuseLight = DiffuseLightCalculation(Vector, WorldNorm);
        SpecularLight = SpecularLightCalculation(Vector, WorldNorm, EyePosition, WorldPos, Shininess);
        AmbientLight = AmbientLightCalculation(LTData.LightColor.w);

        // �Ÿ� ��� ����
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
        
        // worldviewprojection �� �������� �װ� -1~1������ �����Դϱ�? w�� ������������ z���� ������ ���� ���� �˴ϴ�. // ��� ���� -1~1������ ���� �˴ϴ�.
        float3 LightProjection = LightPos.xyz / LightPos.w;
        
        float2 ShadowUV = float2(LightProjection.x * 0.5f + 0.5f, LightProjection.y * -0.5f + 0.5f);
        float fShadowDepth = ShadowTex.Sample(Sampler, float2(ShadowUV.x, ShadowUV.y)).r;
        
       
        // ���� �ܰ��� �ణ ��Ƴ��� 
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
        // �׸��ڰ� �帮��� �κ� ����ŷ, x�ϱ� ���������� ����ŷ��
    }
    
    OutPut.DiffuseTarget = float4(DiffuseLight, 1.0f);
    OutPut.SpecularTarget = float4(SpecularLight, 1.0f);
    OutPut.AmbientTarget = float4(AmbientLight, 1.0f);
    
    return OutPut; // ī�޶� ��� ���� ��ġ �׷����ִ� ���� ��������� ���� ���� �ؽ�ó ����
}