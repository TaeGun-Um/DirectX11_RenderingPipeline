Texture2D DiffuseTex : register(t0);
SamplerState AlwaysSampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Texcoord : TEXCOORD;
};

float4 Merge_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = DiffuseTex.Sample(AlwaysSampler, _Input.Texcoord.xy);
    Color.a = saturate(Color.a);
    
    return Color;
}