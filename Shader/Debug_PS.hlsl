struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

float4 Debug_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = _Input.Color;
    return Color;
}