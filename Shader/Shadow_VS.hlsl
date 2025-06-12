#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 WVPosition : POSITION0;
    float4 WVPPosition : POSITION1;
};

VSOutput Shadow_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;
    float4 InputPos = _Input.Position;
    InputPos.w = 1.0f;
    
    OutPut.WVPosition = mul(_Input.Position, WorldViewMatrix);
    OutPut.WVPosition.w = 1.0f;
    
    OutPut.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    OutPut.WVPPosition = OutPut.Position;
    OutPut.WVPPosition.w = 1.0f;
    
    return OutPut;
}

//OutPut.Position = _Input.Position;
//OutPut.WVPosition = mul(_Input.Position, WorldViewMatrix);
//OutPut.WVPosition.w = 1.0f;
//OutPut.WVPPosition = mul(_Input.Position, WorldViewProjectionMatrix);
//OutPut.WVPPosition.w = 1.0f;