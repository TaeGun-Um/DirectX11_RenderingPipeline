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
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT; // 로컬 접선
    float4 Binormal : BINORMAL; // 로컬 이접선
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldTangent : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

VSOutput PBR_VS(VSInput _Input)
{
    VSOutput Output;
    
    // Position 설정
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
    
    // 월드 공간 기준 노말, 접선, 이접선 구하기
    Output.WorldNormal = mul(float4(_Input.Normal.xyz, 0.0f), WorldMatrix).rgb;
    Output.WorldTangent = mul(float4(_Input.Tangent.xyz, 0.0f), WorldMatrix).rgb;
    Output.WorldBinormal = mul(float4(_Input.Binormal.xyz, 0.0f), WorldMatrix).rgb;
    
    // UV 좌표 설정
    Output.TexCoord = _Input.TexCoord;
    
    // 월드 공간 기준 Position 구하기
    Output.WorldPosition = mul(float4(_Input.Position.xyz, 1.0f), WorldMatrix).rgb;
   
    return Output;
}