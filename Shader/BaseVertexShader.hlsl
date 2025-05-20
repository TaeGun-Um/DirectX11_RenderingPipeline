struct VSInput
{
    float3 Posistion : POSITION;
    float4 Color : COLOR;
};

struct PSInput
{
    float4 Posistion : SV_POSITION;
    float4 Color : COLOR;
};

PSInput main(VSInput _Input)
{
    PSInput Output;
    Output.Posistion = float4(_Input.Posistion, 1.0f);
    Output.Color = _Input.Color;
    return Output;
}