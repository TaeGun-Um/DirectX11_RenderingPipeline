cbuffer TransformData : register(b0)
{
    float4 Scale;
    float4 Rotation;
    float4 Quaternion;
    float4 Position;

    float4x4 ScaleMatrix;
    float4x4 RotationMatrix;
    float4x4 PositionMatrix;
    float4x4 WorldMatrix;

    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4x4 ViewPortMatrix;

    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
}

struct VSInput
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VSOut Basic_VS(VSInput _Input)
{
    VSOut Output;
    float4 WorldPosition = mul(WorldMatrix, _Input.Position);
    float4 ViewPosition = mul(ViewMatrix, WorldPosition);
    Output.Position = mul(ProjectionMatrix, ViewPosition);
    Output.Color = _Input.Color;
    return Output;
}