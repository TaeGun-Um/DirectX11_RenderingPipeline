#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION; // 큐브의 정점 위치 (범위 [-1,1] 의 단위 큐브)
    float4 Texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION; // 최종 클립 공간 위치
    float3 Texcoord : TEXCOORD; // 픽셀 셰이더로 넘길 방향 벡터
};

VSOutput CubemapMerge_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;
    
    OutPut.Position = _Input.Position;
    OutPut.Texcoord = normalize(_Input.Texcoord.xyz);
    
    return OutPut;
}