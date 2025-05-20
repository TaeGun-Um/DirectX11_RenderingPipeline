struct VSInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

PSInput main(VSInput _Input)
{
    PSInput Output;
    Output.Position = float4(_Input.Position, 1.0f);
    Output.Color = _Input.Color;
    return Output;
}