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

VSOutput RenderTargetMerge_VS(VSInput _Input)
{
    VSOutput OutPut = (VSOutput) 0;
    OutPut.Position = _Input.Position;
    OutPut.Texcoord = _Input.Texcoord;
    
    return OutPut;
}