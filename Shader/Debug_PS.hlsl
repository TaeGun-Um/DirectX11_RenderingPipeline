struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

// 픽셀에 저장된 컬러값을 그대로 출력
float4 Debug_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = _Input.Color;
    return Color;
}