struct LightData
{
    float4 LightColor; // RGB(색), w(강도)
    float4 LightWorldPosition;
    float4 LightForwardVector;
    float4 CameraWorldPosition;

    float ShadowTargetSizeX;
    float ShadowTargetSizeY;
    
    float NearDistance;
    float FarDistance;
    float AttenuationValue;
    int LightType;
    bool bIsLightSet;
    
    float4x4 LightViewMatrix;
    float4x4 LightViewInverseMatrix;
    float4x4 LightProjectionMatrix;
    float4x4 LightProjectionInverseMatrix;
    float4x4 LightViewProjectionMatrix;
    float4x4 CameraViewInverseMatrix;
};

cbuffer LightDatas : register(b2)
{
    int LightCount;
    LightData Lights[64];
};

float3 DiffuseLightCalculation(float3 _LightDirection, float3 _Normal)
{
    float3 LightDirection = normalize(_LightDirection); // 빛 계산에 사용하는 LightDirection은 "표면에서 봤을 때 빛이 표면으로 들어오는 방향"을 사용하므로 반대로 뒤집음
    float3 Normal = normalize(_Normal);
    float3 DiffuseLight = saturate(dot(LightDirection, Normal));
    
    return DiffuseLight;
}

float3 SpecularLightCalculation(float3 _LightDirection, float3 _Normal, float3 _CameraWorldPosition, float3 _PixelPosition, float _Shininess)
{
    float3 ReflectionVector = normalize(2.0f * _Normal * dot(_LightDirection, _Normal) - _LightDirection); // 반사벡터
    float3 EyePosition = _CameraWorldPosition;
    float3 EyeDirection = normalize(EyePosition - _PixelPosition);
    float RDotV = max(0.0f, dot(ReflectionVector, EyeDirection));
    float3 SpecularLight = pow(RDotV, _Shininess);
    
    return SpecularLight;
}

float3 AmbientLightCalculation(float _AmbientIntensity)
{
    float3 AmbientLight;
    AmbientLight.x = _AmbientIntensity;
    AmbientLight.y = _AmbientIntensity;
    AmbientLight.z = _AmbientIntensity;
    
    return AmbientLight;
}