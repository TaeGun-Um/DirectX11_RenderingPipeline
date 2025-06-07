struct VSInput
{
    float4 Position : POSITION;
    float4 Texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Texcoord : TEXCOORD;
};

VSOutput Merge_VS(VSInput _Input)
{
    VSOutput OutPut;
    OutPut.Position = _Input.Position;
    OutPut.Texcoord = _Input.Texcoord;
    
    return OutPut;
}