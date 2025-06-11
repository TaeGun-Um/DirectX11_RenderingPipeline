#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 WVPPosition : POSITION;
};

VSOutput Shadow_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;
    float4 InputPos = _Input.Position;
    InputPos.w = 1.0f;
    
    OutPut.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    OutPut.WVPPosition = OutPut.Position;
    OutPut.WVPPosition.w = 1.0f;
    
    return OutPut;
}