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
    float4 DifLight : SV_Target0;
    float4 SpcLight : SV_Target1;
    float4 AmbLight : SV_Target2;
    float4 ShadowTest : SV_Target3;
};

Texture2D PositionTex : register(t0);
Texture2D NormalTex : register(t1);
Texture2D ShadowTex : register(t2);
SamplerState POINTWRAP : register(s0);

LightOutPut DeferredLight_PS(PSInput _Input)
{
    LightOutPut NewOutPut;
    LightData LTData = Lights[0];
    
    float4 WorldViewPos = PositionTex.Sample(POINTWRAP, _Input.Texcoord.xy);
    WorldViewPos.a = 1.0f;
        
        // ���������ϹǷ� �׸��ڵ� �����ؾ����� �Ǵ��Ѵ�.
    float4 WorldPos = mul(float4(WorldViewPos.xyz, 1.0f), LTData.CameraViewInverseMatrix);
    WorldPos.w = 1.0f;
        
        // ���� ���������� ���������� �ٲ��.
    float4 LightPos = mul(WorldPos, LTData.LightViewProjectionMatrix);
        
        // worldviewprojection 
        // �� �������� �װ� -1~1������ �����Դϱ�?
        // w�� ������������ z���� ������ ���� ���� �˴ϴ�.
    float3 LightProjection = LightPos.xyz / LightPos.w;
        // ��� ���� -1~1������ ���� �˴ϴ�.
        
    float2 ShadowUV = float2(LightProjection.x * 0.5f + 0.5f, LightProjection.y * -0.5f + 0.5f);
    float fShadowDepth = ShadowTex.Sample(POINTWRAP, float2(ShadowUV.x, ShadowUV.y)).r;
        
        // ���� �ܰ��� �ణ ��Ƴ��� 
    if (fShadowDepth >= 0.0f
        && 0.001f < ShadowUV.x 
        && 0.999f > ShadowUV.x
        && 0.001f < ShadowUV.y 
        && 0.999f > ShadowUV.y
        && LightProjection.z >= (fShadowDepth + 0.001f)
        )
    {
        NewOutPut.ShadowTest.x = 1.0f;
        NewOutPut.ShadowTest.a = 1.0f;
    }
    else
    {
        NewOutPut.ShadowTest = float4(0.0f, 0.0f, 0.0f, 0.0f);

    }
        
    return NewOutPut;
}