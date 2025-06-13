TextureCube CubeMapTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Texcoord : TEXCOORD;
};

float4 CubemapMerge_PS(PSInput _Input) : SV_TARGET
{
    return CubeMapTex.Sample(Sampler, _Input.Texcoord);
}