cbuffer TransformData : register(b0)
{
    float4 LocalPosition;
    float4 LocalRotation;
    float4 LocalScale;
    float4 LocalQuaternion;
    float4x4 LocalMatrix;

    float4 WorldPosition;
    float4 WorldRotation;
    float4 WorldQuaternion;
    float4 WorldScale;
    float4x4 WorldMatrix;

    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
}

struct VSInput
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

VSOutput Grapics_VS(VSInput _Input)
{
    VSOutput Output;
    // _Input.Position.w = 1.0f;
    
    // Position 설정
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
    
    // UV 좌표 설정
    Output.TexCoord = _Input.TexCoord;
    
    // 월드 공간 기준으로 조명 계산을 진행하기 위해 WorldMatrix만 처리한 Position, Normal을 생성하여 Pixel Shader에 넘겨줌
    Output.WorldPosition = mul(float4(_Input.Position.xyz, 1.0f), WorldMatrix).rgb;
    Output.WorldNormal = mul(float4(_Input.Normal.xyz, 0.0f), WorldMatrix).rgb;
   
    return Output;
}