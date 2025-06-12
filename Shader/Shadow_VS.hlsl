#include "Transform.fx"

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
    VSOutput OutPut = (VSOutput) 0;
    OutPut.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    return OutPut;
}

//OutPut.Position = _Input.Position;
//OutPut.WVPosition = mul(_Input.Position, WorldViewMatrix);
//OutPut.WVPosition.w = 1.0f;
//OutPut.WVPPosition = mul(_Input.Position, WorldViewProjectionMatrix);
//OutPut.WVPPosition.w = 1.0f;