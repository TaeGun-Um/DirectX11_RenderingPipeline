struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Position5 : POSITION;
};

float4 OrthogonalShadow_PS(PSInput _Input) : SV_Target0
{
    return float4(max(0, _Input.Position5.z / _Input.Position5.w), 0.0f, 0.0f, 1.0f);
}