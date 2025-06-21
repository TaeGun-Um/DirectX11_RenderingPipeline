cbuffer RenderingData : register(b3)
{
    int bIsLight;
    int bIsShadow;
    int bIsAnimation;
    int bIsNormal;
    int bIsMetal;
    int bIsRoughNess;
    int bIsAmbient;
    int bIsMask;
    
    float DeltaTime;
    float AccTime;
}