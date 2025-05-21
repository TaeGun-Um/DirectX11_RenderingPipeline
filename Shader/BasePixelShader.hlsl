struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(PSInput _Input) : SV_TARGET
{
    return _Input.Color;
}