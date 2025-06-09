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
    
    // 빛이존재하므로 그림자도 존재해야할지 판단
    float4 WorldPosition = mul(float4(WorldViewPosition.xyz, 1.0f), LTData.CameraViewInverseMatrix);
    WorldPosition.w = 1.0f;
        
    // 빛을 기준으로한 포지션으로 바꿨다.
    float4 LightPosition = mul(WorldPosition, LTData.LightViewProjectionMatrix);
        
    // worldviewprojection 이 곱해지면 w에 곱해지기전의 z값을 보관해 놓은 값이 됨(모든 값은 -1~1사이의 값이 됨)
    float3 LightProjection = LightPosition.xyz / LightPosition.w;
        
    float2 ShadowUV = float2(LightProjection.x * 0.5f + 0.5f, LightProjection.y * -0.5f + 0.5f);
    float fShadowDepth = ShadowTex.Sample(POINTWRAP, float2(ShadowUV.x, ShadowUV.y)).r;
        
    // 가장 외각을 약간 깎아내서 
    if (fShadowDepth >= 0.0f && 0.001f < ShadowUV.x && 0.999f > ShadowUV.x && 0.001f < ShadowUV.y && 0.999f > ShadowUV.y && LightProjection.z >= (fShadowDepth + 0.001f))
    {
        NewOutPut.ShadowTest.x = 1.0f;
        NewOutPut.ShadowTest.a = 1.0f;
    }
    else
    {
        // 그림자 바깥(Light), x 채널에 0을 넣어 “그림자 아님” 표시, a 채널은 그대로 1로 유지
        NewOutPut.ShadowTest.x = 0.0f;
        NewOutPut.ShadowTest.a = 1.0f;
    }
    
    // 카메라 행렬 빛의 위치 그려져있는 빛을 기반으로한 깊이 버퍼 텍스처 리턴
    return NewOutPut;
}