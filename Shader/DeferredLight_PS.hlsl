struct LightData
{
    float4 LightColor; // RGB(��), w(����)
    float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
    float4 LightForwardVector; // ���� �Ի� ����
    float4 CameraWorldPosition; // ���� ���� ���� ��ġ

    float ShadowTargetSizeX;
    float ShadowTargetSizeY;
    
    float NearDistance;
    float FarDistance;
    float AttenuationValue;
    int LightType;
    bool bIsLightSet;
    
    float4x4 LightViewMatrix;
    float4x4 LightViewInverseMatrix;
    float4x4 LightProjectionMatrix;
    float4x4 LightProjectionInverseMatrix;
    float4x4 LightViewProjectionMatrix;
    float4x4 CameraViewInverseMatrix;
};

cbuffer LightDatas : register(b1)
{
    int LightCount;
    LightData Lights[64];
};

struct PSInput
{
    float4 Position : SV_Position;
    float4 Texcoord : TEXCOORD;
};

struct LightOutPut
{
    float4 ShadowTest : SV_Target0;
    // float4 DifLight : SV_Target1;
    // float4 SpcLight : SV_Target2;
    // float4 AmbLight : SV_Target3;
};

Texture2D PositionTex : register(t0);
Texture2D ShadowTex : register(t2);
SamplerState POINTWRAP : register(s0);

LightOutPut DeferredLight_PS(PSInput _Input)
{
    LightOutPut NewOutPut;
        
    float4 WorldViewPosition = PositionTex.Sample(POINTWRAP, _Input.Texcoord.xy);
    WorldViewPosition.a = 1.0f;
    
    if (0 >= WorldViewPosition.a)
    {
        clip(-1);
    }
        
    LightData LTData = Lights[0];
    
    // ���������ϹǷ� �׸��ڵ� �����ؾ����� �Ǵ�
    float4 WorldPosition = mul(float4(WorldViewPosition.xyz, 1.0f), LTData.CameraViewInverseMatrix);
    WorldPosition.w = 1.0f;
        
    // ���� ���������� ���������� �ٲ��.
    float4 LightPosition = mul(WorldPosition, LTData.LightViewProjectionMatrix);
        
    // worldviewprojection �� �������� w�� ������������ z���� ������ ���� ���� ��(��� ���� -1~1������ ���� ��)
    float3 LightProjection = LightPosition.xyz / LightPosition.w;
        
    float2 ShadowUV = float2(LightProjection.x * 0.5f + 0.5f, LightProjection.y * -0.5f + 0.5f);
    float fShadowDepth = ShadowTex.Sample(POINTWRAP, float2(ShadowUV.x, ShadowUV.y)).r;
        
    // ���� �ܰ��� �ణ ��Ƴ��� 
    if (fShadowDepth >= 0.0f && 0.001f < ShadowUV.x && 0.999f > ShadowUV.x && 0.001f < ShadowUV.y && 0.999f > ShadowUV.y && LightProjection.z >= (fShadowDepth + 0.001f))
    {
        NewOutPut.ShadowTest.x = 1.0f;
        NewOutPut.ShadowTest.a = 1.0f;
    }
    else
    {
        // �׸��� �ٱ�(Light), x ä�ο� 0�� �־� ���׸��� �ƴԡ� ǥ��, a ä���� �״�� 1�� ����
        NewOutPut.ShadowTest.x = 0.0f;
        NewOutPut.ShadowTest.a = 1.0f;
    }
    
    // ī�޶� ��� ���� ��ġ �׷����ִ� ���� ��������� ���� ���� �ؽ�ó ����
    return NewOutPut;
}