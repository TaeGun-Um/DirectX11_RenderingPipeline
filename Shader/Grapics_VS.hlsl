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
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

VSOutput Grapics_VS(VSInput _Input)
{
    VSOutput Output;
    // _Input.Position.w = 1.0f;
    
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
    // Output.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    
    Output.Color = _Input.Color;
    Output.TexCoord = _Input.TexCoord;
    
    // Output.Normal = _Input.Normal;
    // 2) 로컬 법선(_Input.Normal.xyz)을 “월드→뷰 공간” 법선으로 변환
    //    (여기서 비균일 스케일 없다는 가정 하에 WorldMatrix*ViewMatrix 상위 3×3만 사용)
    float4x4 worldView4x4 = mul(WorldMatrix, ViewMatrix);
    float3x3 worldView3x3 = (float3x3) worldView4x4;
    float3 normalVS3 = normalize(mul(_Input.Normal.xyz, worldView3x3));

    // 3) float3 → float4 로 명시적 확장. w 성분은 방향 벡터이므로 0.0f 로 채웁니다.
    Output.Normal = float4(normalVS3, 0.0f);
   
    return Output;
}