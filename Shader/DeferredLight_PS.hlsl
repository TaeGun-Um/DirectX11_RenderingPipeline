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
        
    float4 Position = PositionTex.Sample(POINTWRAP, _Input.Texcoord.xy);
    
    if (0 >= Position.a)
    {
        clip(-1);
    }
    
    float4 Normal = NormalTex.Sample(POINTWRAP, _Input.Texcoord.xy);
    
    LightData LTData = Lights[0];
             
    float4 DiffuseRatio = (float4) 0.0f;
    float3 LightDirection = normalize(-LTData.LightForwardVector.xyz);
    Normal.xyz = normalize(Normal.xyz); // N
    DiffuseRatio = max(0.0f, dot(Normal.xyz, LightDirection.xyz));
    
    float4 SpacularRatio = (float4) 0.0f;
    Normal.xyz = normalize(Normal.xyz); // N
    
    float3 ReflectionVector = normalize(2.0f * Normal.xyz * dot(LightDirection, Normal.xyz) - LightDirection); // 반사벡터
    float3 EyeDirection = normalize(LTData.CameraWorldPosition.xyz - Position.xyz); // L
    float Result = max(0.0f, dot(ReflectionVector.xyz, EyeDirection.xyz));
    SpacularRatio.xyzw = pow(Result, 32.0f);
    
    float4 AmbientRatio = LTData.LightColor.w;
    
    // 디뷰즈 라이트가 비춰진 곳이라면
    if (DiffuseRatio.x > 0.0f)
    {
        float4 WorldViewPos = Position;
        WorldViewPos.a = 1.0f;
        // 빛이존재하므로
        // 그림자도 존재해야할지 판단한다.
        // 어느 world 
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
        if (
            fShadowDepth >= 0.0f
            && 0.001f < ShadowUV.x && 0.999f > ShadowUV.x
            && 0.001f < ShadowUV.y && 0.999f > ShadowUV.y
            && LightProjection.z >= (fShadowDepth + 0.001f)
            )
        {
            NewOutPut.ShadowTest.x = 1.0f;
            NewOutPut.ShadowTest.a = 1.0f;
            // DiffuseRatio *= 0.01f;
            // SpacularRatio *= 0.01f;
        }
        else
        {
            // 그림자 바깥(Light)
            NewOutPut.ShadowTest.x = 0.0f;
            NewOutPut.ShadowTest.a = 1.0f;
            // → x 채널에 0을 넣어 “그림자 아님” 표시, a 채널은 그대로 1로 유지
        }
    }
    
    NewOutPut.DifLight = DiffuseRatio;
    NewOutPut.SpcLight = SpacularRatio;
    NewOutPut.AmbLight = AmbientRatio;
    
    // 카메라 행렬
    // 빛의 위치
    // 그려져있는 빛을 기반으로한 깊이 버퍼 텍스처.
    return NewOutPut;
}