struct PSInput
{
    float4 Position : SV_POSITION;
};

float4 Shadow_PS(PSInput _Input) : SV_TARGET
{
    return float4(max(0.0f, _Input.Position.z / _Input.Position.w), 0.0f, 0.0f, 1.0f);
}

//return float4(max(0.0f, length(_Input.WVPosition.xyz)), 0.0f, 0.0f, 1.0f);
//return float4(max(0.0f, _Input.WVPPosition.z / _Input.WVPPosition.w), 0.0f, 0.0f, 1.0f);
//return float4(max(0.0f, length(_Value.ShadowPos.xyz)), 0.0f, 0.0f, 1.0f);