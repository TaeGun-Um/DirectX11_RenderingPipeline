//struct LightData
//{
//    float4 LightColor; // RGB(��), w(����)
//    float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
//    float4 LightForwardVector; // ���� �Ի� ����
//    float4 CameraWorldPosition; // ���� ���� ���� ��ġ

//    float NearDistance;
//    float FarDistance;
//    int LightType;
//};

//cbuffer LightDatas : register(b1)
//{
//    int LightCount;
//    LightData Lights[64];
//};

cbuffer LightData : register(b1)
{
    float4 LightColor; // RGB(��), w(����)
    float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
    float4 LightForwardVector; // ���� �Ի� ����
    float4 CameraWorldPosition; // ���� ���� ���� ��ġ

    float NearDistance;
    float FarDistance;
    int LightType;
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
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    // ������� ���� �ȼ�(ǥ��) ��ġ�� ���� ���� ����
    float3 PixelPosition = _Input.WorldPosition;
    float3 PixelNormal = normalize(_Input.WorldNormal);
    
    // ������� ���� �ü� ��ġ�� �ü��� ���� ���� ����
    float3 EyePosition = CameraWorldPosition.xyz;
    float3 EyeDirection = normalize(EyePosition - _Input.WorldPosition);
    
    // ����и�
    float3 FinalLight = float3(0.0f, 0.0f, 0.0f);
    float3 LightDirection = normalize(-LightForwardVector.xyz); // �� ��꿡 ����ϴ� LightDirection�� "ǥ�鿡�� ���� �� ���� ǥ������ ������ ����"�� ����ϹǷ� �ݴ�� ������
    float3 ReflectionVector = normalize(2.0f * PixelNormal * dot(LightDirection, PixelNormal) - LightDirection); // �ݻ纤��
    float Shininess = 32.0f; // Shininess�� ���Ƿ� 32 ����
    float AmbientIntensity = LightColor.w;
    
    // ���� ���� �÷� (RGB)
    float3 AccumLightColor = float3(0, 0, 0);
    
    if (LightType == 0) // Directional Light
    {
        // Diffuse Light ���
        float DiffuseLight = saturate(dot(LightDirection, PixelNormal));
        
        // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
        float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
        float3 SpecularLight = pow(RDotV, Shininess);
        
        // Ambient Light ���, ������ LightColor�� w�� ���
        
        float3 AmbientLight = AmbientIntensity;
        
        AccumLightColor += LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight);
    }
    else if (LightType == 1) // ����Ʈ ����Ʈ
    {
        float3 LightPosition = LightWorldPosition.xyz;
        
        // �ȼ��� ��ġ�� ���� ��ġ ���� �Ÿ� �� ���� ���ϱ�
        float3 PixelToLight = LightPosition - PixelPosition;
        float Distance = length(PixelToLight);
        
        if (Distance >= NearDistance && Distance <= FarDistance)
        {
            // �ȼ����� �������� ���ϴ� ���� ����
            float3 Vector = normalize(PixelToLight);
            
            // ����(Attenuation) ���� = 1/(c0 + c1��d + c2��d��)
            float C0 = 1.0f;
            float C1 = 0.0f;
            float C2 = 1.0f / (FarDistance * FarDistance);
            float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);

            // Diffuse Light ���
            float DiffuseLight = saturate(dot(PixelNormal, Vector));

            // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(L, N)
            float3 ReflectionVector = normalize(2.0f * PixelNormal * dot(Vector, PixelNormal) - Vector);
            float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
            float3 SpecularLight = pow(RDotV, Shininess);

            // Ambient Light ���, ������ LightColor�� w�� ���
            float3 AmbientLight = AmbientIntensity;

            // 6.2.9) ����
            AccumLightColor += LightColor.xyz * (DiffuseLight + SpecularLight + AmbientLight) * Attenuation;
        }
    }
    
    Albedo.rgb *= AccumLightColor;
    return Albedo;
}