#include "LightData.fx"

Texture2D BaseColorTex : register(t0); // 텍스처 자원
TextureCube ReflectionTexture : register(t1);
SamplerState Sampler : register(s0); // 샘플러
SamplerState CubeMapSampler : register(s1); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION0;
    float3 WorldNormal : NORMAL;
    float4 CameraWorldPosition : POSITION1;
};

struct PSOutPut
{
    float4 MeshTarget : SV_TARGET0;
    float4 PositionTarget : SV_TARGET1; // World Position Target
    float4 NormalTarget : SV_TARGET2; // World Normal Target
};

 // 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
 // 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
PSOutPut GraphicsCUBE_PS(PSInput _Input) : SV_TARGET
{
    PSOutPut Output = (PSOutPut) 0;

    float3 CameraPos = _Input.CameraWorldPosition;
    float3 ViewDir = normalize(CameraPos - _Input.WorldPosition);
    float3 Normal = normalize(_Input.WorldNormal);
    
    float3 ReflectDir = normalize(2.0f * Normal * dot(ViewDir, Normal) - ViewDir);
    float4 ReflectionColor = ReflectionTexture.Sample(CubeMapSampler, ReflectDir);
    
    float metallic = 1.0f;

    Output.MeshTarget = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    Output.MeshTarget += float4(lerp(float3(0, 0, 0), ReflectionColor.rgb * 0.5f, metallic), 0.0f);

    Output.PositionTarget = float4(_Input.WorldPosition, 1.0f);
    Output.NormalTarget = float4(_Input.WorldNormal, 1.0f);

    return Output;
}