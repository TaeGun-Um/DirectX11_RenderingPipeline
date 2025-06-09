#include "Transform.fx"

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

VSOutput NonGStatic_VS(VSInput _Input)
{
    VSOutput Output;
    
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
    
    Output.Color = _Input.Color;
    Output.TexCoord = _Input.TexCoord;
    Output.Normal = _Input.Normal;
   
    return Output;
}