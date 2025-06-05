cbuffer LightData : register(b0)
{
    float4 LightColor; // RGB(색), w(강도)
	
    float4 LightWorldPosition; // 라이트 월드 공간 위치
    float4 LightViewPosition; // 라이트 뷰 공간 위치
    float4 LightForwardVector; // 월드 입사 벡터
    float4 LightBackwordVector; // 월드 입사 벡터 반대 방향
    float4 LightViewForwardVector; // 뷰 입사 벡터

    float4 CameraWorldPosition; // 시점 월드 공간 위치
    float4 CameraViewPosition; // 시점 뷰 공간 위치
    float4 CameraForwardVector; // 월드 시점 벡터
    float4 CameraViewForwardVector; // 뷰 시점 벡터

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

float4 Grapics_PS(PSInput _Input) : SV_TARGET
{
    // 텍스처 색상
    float4 Albedo = BaseColorTex.Sample(Sampler, _Input.TexCoord);
    
    // 월드 공간 기준 빛 방향과 표면 법선 벡터 구하기
    float3 LightDirection = normalize(-LightForwardVector.xyz);
    float3 SurfaceNormal = normalize(_Input.WorldNormal);
    
    // Diffuse Light 계산
    float DiffuseLight = saturate(dot(LightDirection, SurfaceNormal)); // max(dot(N,L), 0)로 해도 됨
    
    // Specular Light 계산, Phong 모델을 사용하기 때문에 R = reflect(-L, N)
    float3 LightDirection2 = normalize(LightForwardVector.xyz);
    float3 Reflection = reflect(LightDirection2, SurfaceNormal);
    float3 EyeDirection = normalize(CameraWorldPosition.xyz - _Input.WorldPosition);
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