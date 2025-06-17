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
    
    //float4 HSVColor = { 1.f, 0.55f, 0.5f, 0.5f };
    
    //float saturation = HSVColor.r * 2;
    //float brightness = HSVColor.g * 2 - 1;
    //float contrast = HSVColor.b * 2;
    
    //Color.rgb = (Color.rgb - 0.5f) * contrast + 0.5f;
    //Color.rgb = Color.rgb + brightness;
    
    return Color;
}
