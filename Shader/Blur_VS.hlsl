struct VSInput
{
    float4 Position : POSITION;
    float4 Texcoord : TEXCOORD;
};

struct VSOutPut
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

VSOutPut Blur_VS(VSInput _Input)
{
    VSOutPut OutPut = (VSOutPut) 0;
    OutPut.Position = _Input.Position;
    OutPut.Texcoord = _Input.Texcoord.xy;
    
    return OutPut;
}