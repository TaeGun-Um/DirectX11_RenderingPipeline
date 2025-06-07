struct PSInput
{
    float4 Position : SV_POSITION;
    float4 ShadowPosition : POSITION;
};

float4 PerspectiveShadow_PS(PSInput _Input) : SV_Target0
{
    return float4(max(0.0f, length(_Input.ShadowPosition.xyz)), 0.0f, 0.0f, 1.0f);
}