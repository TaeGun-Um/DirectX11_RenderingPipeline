struct VSInput
{
    float4 Position : SV_POSITION;
    float4 Pos5 : POSITION5;
};

float4 Shadow_PS(VSInput _Value) : SV_TARGET
{
    return float4(max(0.0f, _Value.Pos5.z / _Value.Pos5.w), 0.0f, 0.0f, 1.0f);
}