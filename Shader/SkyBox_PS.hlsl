TextureCube CubeMapTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

float4 SkyBox_PS(PSInput _Input) : SV_TARGET
{
    float4 Color = CubeMapTex.Sample(Sampler, _Input.TexCoord);
    return Color;
}
