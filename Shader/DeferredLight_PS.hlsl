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

Texture2D PositionTex : register(t0); // G-Buffer: ��-�����̽� ��ġ(x,y,z) + 1
Texture2D NormalTex : register(t1); // G-Buffer: ��-�����̽� ����(x,y,z) + 1
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
    float Shininess = 32.0f; // Shininess�� ���Ƿ� 32 ����
     
    DiffuseLight = DiffuseLightCalculation(LightDirection, WorldNorm);
    SpecularLight = SpecularLightCalculation(LightDirection, WorldNorm, CameraWorldPos, WorldNorm, Shininess);
    AmbientLight = AmbientLightCalculation(LTData.LightColor.w);
    
    if (LTData.LightType == 1) // ����Ʈ ����Ʈ
    {
        // �ȼ��� ��ġ�� ���� ��ġ ���� �Ÿ� �� ���� ���ϱ�
        float Distance = length(LightDirection);
  
        // ����(Attenuation) ���� = 1/(c0 + c1��d + c2��d��)
        float C0 = 1.0f;
        float C1 = 0.0f;
        float C2 = LTData.AttenuationValue / (LTData.FarDistance * LTData.FarDistance); // �Ÿ��� ���� ������ ���, ���⸦ �����ϸ� ���谡 ����
        Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);
    }
    
    //// DiffuseLight�� �������� ���̶��
    //if (DiffuseLight.x > 0.0f)
    //{
    //    // ���������ϹǷ� �׸��ڵ� �����ؾ����� �Ǵ�(���� ���������� ���������� ����)
    //    float4 WVPPixelPos = mul(WorldPos, LTData.LightViewProjectionMatrix);
        
    //    // worldviewprojection �� �������� �װ� -1~1������ �����Դϱ�? w�� ������������ z���� ������ ���� ���� �˴ϴ�. // ��� ���� -1~1������ ���� �˴ϴ�.
    //    float3 PixelProjection = WVPPixelPos.xyz / WVPPixelPos.w;
        
    //    float2 ShadowUV = float2(PixelProjection.x * 0.5f + 0.5f, PixelProjection.y * -0.5f + 0.5f);
    //    float ShadowDepth = ShadowTex.Sample(PointWrap, float2(ShadowUV.x, ShadowUV.y)).r;
        
    //    // ���� �ܰ��� �ణ ��Ƴ��� 
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
    //        // �׸��� �ٱ�(Light)  �� x ä�ο� 0�� �־� ���׸��� �ƴԡ� ǥ��, a ä���� �״�� 1�� ����
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
    
    return OutPut; // ī�޶� ��� ���� ��ġ �׷����ִ� ���� ��������� ���� ���� �ؽ�ó ����
}