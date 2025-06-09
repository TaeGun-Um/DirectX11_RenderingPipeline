#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION;
    // float4 BLENDWEIGHT : BLENDWEIGHT;
    // int4 BLENDINDICES : BLENDINDICES;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Pos5 : POSITION5;
};

VSOutput Shadow_VS(VSInput _Input)
{
    VSOutput OutPut;
    float4 InputPos = _Input.Position;
    InputPos.w = 1.0f;
    
    //if (IsAnimation != 0)
    //{
    //    Skinning(InputPos, _Input.BLENDWEIGHT, _Input.BLENDINDICES, ArrAniMationMatrix);
    //    InputPos.w = 1.0f;
    //}
    
    OutPut.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    OutPut.Pos5 = OutPut.Position;
    OutPut.Pos5.w = 1.0f;
    
    return OutPut;
}