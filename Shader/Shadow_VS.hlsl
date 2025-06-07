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
};

struct VSOutput
{
    float4 Position : SV_POSITION;
};

VSOutput Shadow_VS(VSInput _Input)
{
    VSOutput Output;
    Output.Position = mul(_Input.Position, WorldViewProjectionMatrix);
   
    return Output;
}