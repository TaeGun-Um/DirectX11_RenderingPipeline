#include "Transform.fx"

struct VSInput
{
    float4 Position : POSITION; // ť���� ���� ��ġ (���� [-1,1] �� ���� ť��)
    float4 Texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION; // ���� Ŭ�� ���� ��ġ
    float3 Texcoord : TEXCOORD; // �ȼ� ���̴��� �ѱ� ���� ����
};

VSOutput CubemapMerge_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;
    
    OutPut.Position = _Input.Position;
    OutPut.Texcoord = normalize(_Input.Texcoord.xyz);
    
    return OutPut;
}