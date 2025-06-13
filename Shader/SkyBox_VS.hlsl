#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

VSOutput SkyBox_VS(VSInput _Input)
{
    VSOutput Output;
    float4x4 ViewMat = ViewMatrix;
    ViewMat._41 = 0;
    ViewMat._42 = 0;
    ViewMat._43 = 0;
    
    float4 VPosition = mul(_Input.Position, ViewMat);
    Output.Position = mul(VPosition, ProjectionMatrix);
    Output.TexCoord = _Input.Position.xyz;
    
    return Output;
}