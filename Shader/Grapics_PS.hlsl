cbuffer LightData : register(b0)
{
    float4 LightColor; // RGB(��), w(����)
	
    float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
    float4 LightViewPosition; // ����Ʈ �� ���� ��ġ
    float4 LightForwardVector; // ���� �Ի� ����
    float4 LightBackwordVector; // ���� �Ի� ���� �ݴ� ����
    float4 LightViewForwardVector; // �� �Ի� ����

    float4 CameraWorldPosition; // ���� ���� ���� ��ġ
    float4 CameraViewPosition; // ���� �� ���� ��ġ
    float4 CameraForwardVector; // ���� ���� ����
    float4 CameraViewForwardVector; // �� ���� ����

    float LightNear;
    float LightFar;
    int LightType;
}

Texture2D BaseColorTex : register(t0); // �ؽ�ó �ڿ�
SamplerState Sampler : register(s0); // ���÷�

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

float4 Grapics_PS(PSInput _Input) : SV_TARGET
{
    // �ؽ�ó ����
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    // ���� ���� ���� �� ����� ǥ�� ���� ���� ���ϱ�
    float3 LightDirection = normalize(-LightForwardVector.xyz);
    float3 SurfaceNormal = normalize(_Input.WorldNormal);
    
    // Diffuse Light ���
    float DiffuseLight = saturate(dot(LightDirection, SurfaceNormal)); // max(dot(N,L), 0)�� �ص� ��
    
    // Specular Light ���, Phong ���� ����ϱ� ������ R = reflect(-L, N)
    float3 LightDirection2 = normalize(LightForwardVector.xyz);
    float3 Reflection = reflect(LightDirection2, SurfaceNormal);
    float3 EyeDirection = normalize(CameraWorldPosition.xyz - _Input.WorldPosition);
    float RDotV = max(0.0f, dot(Reflection, EyeDirection));
    float Shininess = 32.0f; // Shininess�� ���Ƿ� 32 ����
    float3 SpecularLight = pow(RDotV, Shininess);
    
    // Ambient Light ���, ������ LightColor�� w�� ���
    float AmbientIntensity = LightColor.w;
    float3 AmbientLight = AmbientIntensity;
    
    float3 FinalLight = DiffuseLight + SpecularLight + AmbientLight;
    Albedo.rgb *= FinalLight;
    
    return Albedo;
}