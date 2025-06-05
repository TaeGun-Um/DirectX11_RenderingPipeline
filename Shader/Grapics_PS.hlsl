cbuffer LightData : register(b0)
{
    float4 LightColor; // RGB(색), w(강도)
    float4 LightForwardVector; // 월드 입사 벡터
    float4 CameraWorldPosition; // 시점 월드 공간 위치

    float LightNear;
    float LightFar;
    int LightType;
}

Texture2D BaseColorTex : register(t0); // 텍스처 자원
SamplerState Sampler : register(s0); // 샘플러

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

// 각 벡터에 normalize를 해주는 이유는, 명시적으로 normalize된 벡터를 넣어줬다 하더라도 
// 임의의 값이 어떻게 들어올지 모르기 때문에 그냥 해주는것(안정성을 위한 처리라고 보면 됨)
float4 Grapics_PS(PSInput _Input) : SV_TARGET
{
    // 텍스처 색상
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    // 월드 공간 기준 빛 방향과 표면 법선 벡터 구하기
    float3 LightDirection = normalize(-LightForwardVector.xyz); // 빛 계산에 사용하는 LightDirection은 "표면에서 봤을 때 빛이 표면으로 들어오는 방향"을 사용하므로 반대로 뒤집음
    float3 SurfaceNormal = normalize(_Input.WorldNormal);
    
    // Diffuse Light 계산
    float DiffuseLight = saturate(dot(LightDirection, SurfaceNormal)); // max(dot(N,L), 0)로 해도 됨
    
    // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(L, N)
    float3 Reflection = normalize(2.0f * SurfaceNormal * dot(LightDirection, SurfaceNormal) - LightDirection);
    float3 EyeDirection = normalize(CameraWorldPosition.xyz - _Input.WorldPosition); // 점 위치에서 카메라를 카리키는 단위 벡터 생성
    float RDotV = max(0.0f, dot(Reflection, EyeDirection));
    float Shininess = 32.0f; // Shininess는 임의로 32 고정
    float3 SpecularLight = pow(RDotV, Shininess);
    
    // Ambient Light 계산, 강도는 LightColor의 w값 사용
    float AmbientIntensity = LightColor.w;
    float3 AmbientLight = AmbientIntensity;
    
    float3 FinalLight = DiffuseLight + SpecularLight + AmbientLight;
    Albedo.rgb *= FinalLight;
    
    return Albedo;
}