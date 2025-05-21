//struct VSInput
//{
//    float3 Position : POSITION;
//    float4 Color : COLOR;
//};

//struct PSInput
//{
//    float4 Position : SV_POSITION;
//    float4 Color : COLOR;
//};

//PSInput main(VSInput _Input)
//{
//    PSInput Output;
//    Output.Position = float4(_Input.Position, 1.0f);
//    Output.Color = _Input.Color;
//    return Output;
//}

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
    float4x4 LocalMatrix;
    
    float4x4 View;
    float4x4 Projection;
    float4x4 ViewPort;
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

VSOut main(VSInput _Input)
{
    VSOut Output;
    float4 WorldPosition = mul(_Input.Position, WorldMatrix);
    float4 ViewPosition = mul(WorldPosition, View);
    Output.Position = mul(ViewPosition, Projection);
    Output.Color = _Input.Color;
    return Output;
}