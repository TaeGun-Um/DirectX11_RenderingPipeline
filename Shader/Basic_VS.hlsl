cbuffer TransformData : register(b0)
{
    float4x4 WorldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
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