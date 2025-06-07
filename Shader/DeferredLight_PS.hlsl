struct LightData
{
    float4 LightColor; // RGB(색), w(강도)
    float4 LightWorldPosition; // 라이트 월드 공간 위치
    float4 LightForwardVector; // 월드 입사 벡터
    float4 CameraWorldPosition; // 시점 월드 공간 위치

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
        
        // 빛이존재하므로 그림자도 존재해야할지 판단한다.
    float4 WorldPos = mul(float4(WorldViewPos.xyz, 1.0f), LTData.CameraViewInverseMatrix);
    WorldPos.w = 1.0f;
        
        // 빛을 기준으로한 포지션으로 바꿨다.
    float4 LightPos = mul(WorldPos, LTData.LightViewProjectionMatrix);
        
        // worldviewprojection 
        // 이 곱해지면 그건 -1~1사이의 공간입니까?
        // w에 곱해지기전의 z값을 보관해 놓은 값이 됩니다.
    float3 LightProjection = LightPos.xyz / LightPos.w;
        // 모든 값은 -1~1사이의 값이 됩니다.
        
    float2 ShadowUV = float2(LightProjection.x * 0.5f + 0.5f, LightProjection.y * -0.5f + 0.5f);
    float fShadowDepth = ShadowTex.Sample(POINTWRAP, float2(ShadowUV.x, ShadowUV.y)).r;
        
        // 가장 외각을 약간 깎아내서 
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