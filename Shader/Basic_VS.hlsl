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

struct VSOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

VSOut Basic_VS(VSInput _Input)
{
    VSOut Output;
    _Input.Position.w = 1.0f;
    Output.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    Output.Color = _Input.Color;
    Output.TexCoord = _Input.TexCoord;
    Output.Normal = _Input.Normal;
   
    return Output;
}