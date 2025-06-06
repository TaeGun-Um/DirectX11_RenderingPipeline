struct LightData
{
    float4 LightColor; // RGB(��), w(����)
    float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
    float4 LightForwardVector; // ���� �Ի� ����
    float4 CameraWorldPosition; // ���� ���� ���� ��ġ

    float NearDistance;
    float FarDistance;
    float AttenuationValue;
    int LightType;
    bool bIsLightSet;
};

cbuffer LightDatas : register(b1)
{
    int LightCount;
    LightData Lights[64];
};

Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
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
        
        // ����и�
        float3 FinalLight = float3(0.0f, 0.0f, 0.0f);
        float3 LightDirection = normalize(-LTData.LightForwardVector.xyz); // �� ��꿡 ����ϴ� LightDirection�� "ǥ�鿡�� ���� �� ���� ǥ������ ������ ����"�� ����ϹǷ� �ݴ�� ������
        
        float Shininess = 32.0f; // Shininess�� ���Ƿ� 32 ����
        float AmbientIntensity = LTData.LightColor.w;
        
        // ������� ���� �ü� ��ġ�� �ü��� ���� ���� ����
        float3 EyePosition = LTData.CameraWorldPosition.xyz;
        float3 EyeDirection = normalize(EyePosition - _Input.WorldPosition);
        
        if (LTData.LightType == 0) // Directional Light
        {
            // Diffuse Light ���
            float DiffuseLight = saturate(dot(LightDirection, PixelNormal));
        
            // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
            float3 ReflectionVector = normalize(2.0f * PixelNormal * dot(LightDirection, PixelNormal) - LightDirection); // �ݻ纤��
            float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
            float3 SpecularLight = pow(RDotV, Shininess);
        
            // Ambient Light ���, ������ LightColor�� w�� ���
            float3 AmbientLight = AmbientIntensity;
        
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
        }
        else if (LTData.LightType == 1) // ����Ʈ ����Ʈ
        {
            float3 LightPosition = LTData.LightWorldPosition.xyz;
        
            // �ȼ��� ��ġ�� ���� ��ġ ���� �Ÿ� �� ���� ���ϱ�
            float3 PixelToLight = LightPosition - PixelPosition;
            float Distance = length(PixelToLight); // Distance�� Near, Far �ø��ϸ� ��մ°� �� �� ����, �ؿ� �ּ��� ���
            //if (Distance >= LTData.NearDistance && Distance <= LTData.FarDistance)
            
            // �ȼ����� �������� ���ϴ� ���� ����
            float3 Vector = normalize(PixelToLight);
            
            // ����(Attenuation) ���� = 1/(c0 + c1��d + c2��d��)
            float C0 = 1.0f;
            float C1 = 0.0f;
            float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // �Ÿ��� ���� ������ ���, ���⸦ �����ϸ� ���谡 ����
            float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);

            // Diffuse Light ���
            float DiffuseLight = saturate(dot(PixelNormal, Vector));

            // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
            float3 ReflectionVector = normalize(2.0f * PixelNormal * dot(Vector, PixelNormal) - Vector);
            float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
            float3 SpecularLight = pow(RDotV, Shininess);

            // Ambient Light ���, ������ LightColor�� w�� ���
            float3 AmbientLight = AmbientIntensity;

            // ����
            AccumLightColor += LTData.LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight) * Attenuation;
        }
    }
    
    Albedo.rgb *= AccumLightColor;
    return Albedo;
}