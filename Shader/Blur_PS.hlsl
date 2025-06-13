// 가우시안 커널
// Total = 16+64+96+64+16 = 256
static float Gau[5][5] =
{
    { 1, 4, 6, 4, 1 },
    { 4, 16, 24, 16, 4 },
    { 6, 24, 36, 24, 6 },
    { 4, 16, 24, 16, 4 },
    { 1, 4, 6, 4, 1 }
};

// 7×7 가우시안 커널 (Pascal row 6: 1, 6, 15, 20, 15, 6, 1)
// 전체 가중치 합 = (1+6+15+20+15+6+1)² = 64² = 4096
static const float Gau7[7][7] =
{
    { 1, 6, 15, 20, 15, 6, 1 },
    { 6, 36, 90, 120, 90, 36, 6 },
    { 15, 90, 225, 300, 225, 90, 15 },
    { 20, 120, 300, 400, 300, 120, 20 },
    { 15, 90, 225, 300, 225, 90, 15 },
    { 6, 36, 90, 120, 90, 36, 6 },
    { 1, 6, 15, 20, 15, 6, 1 }
};

// 9×9 가우시안 커널 (Pascal row 8: 1, 8, 28, 56, 70, 56, 28, 8, 1)
// 전체 가중치 합 = (1+8+28+56+70+56+28+8+1)² = 256² = 65536
static const float Gau9[9][9] =
{
    { 1, 8, 28, 56, 70, 56, 28, 8, 1 },
    { 8, 64, 224, 448, 560, 448, 224, 64, 8 },
    { 28, 224, 784, 1568, 1960, 1568, 784, 224, 28 },
    { 56, 448, 1568, 3136, 3920, 3136, 1568, 448, 56 },
    { 70, 560, 1960, 3920, 4900, 3920, 1960, 560, 70 },
    { 56, 448, 1568, 3136, 3920, 3136, 1568, 448, 56 },
    { 28, 224, 784, 1568, 1960, 1568, 784, 224, 28 },
    { 8, 64, 224, 448, 560, 448, 224, 64, 8 },
    { 1, 8, 28, 56, 70, 56, 28, 8, 1 }
};

cbuffer FrameData : register(b0)
{
    float4 ScreenSize; // 화면 크기
    float AccTime; // 실행된 시간
};

Texture2D DiffuseTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

// 5x5 가우시안 블러
//float4 Blur_PS(PSInput _Input) : SV_TARGET
//{
//    // 픽셀 하나 사이즈는 몇입니까
//    float2 PixelSize = float2(1.0f / ScreenSize.x, 1.0f / ScreenSize.y);
    
//    // UV 중심에서 2픽셀씩 왼쪽위로 이동하여 가우시안 시작 지점 정하기
//    float2 PixelUvCenter = _Input.Texcoord.xy;
//    float2 StartUV = _Input.Texcoord.xy + (-PixelSize * 2.0f);
//    float2 CurUV = StartUV;
//    float4 ResultColor = (float4) 0.0f;
 
//    // 5×5 샘플링 & 가중치 누적
//    for (int y = 0; y < 5; ++y)
//    {
//        for (int x = 0; x < 5; ++x)
//        {
//            ResultColor += DiffuseTex.Sample(Sampler, CurUV.xy) * Gau[y][x];
//            CurUV.x += PixelSize.x;
//        }
        
//        CurUV.x = StartUV.x;
//        CurUV.y += PixelSize.y;
//    }
    
//    // 가중치 총합 256 으로 정규화
//    // 이렇게 해야 모든 샘플 값이 커널 가중치만큼 더해져서 전체 이미지가 지나치게 밝아지거나 어두워지는 것 방지
//    ResultColor /= 256.0f;
    
//    return ResultColor;
//}

// 7x7 가우시안 블러
//float4 Blur_PS(PSInput _Input) : SV_TARGET
//{
//    // 픽셀 하나 사이즈는 몇입니까
//    float2 PixelSize = float2(1.0f / ScreenSize.x, 1.0f / ScreenSize.y);
    
//    // UV 중심에서 2픽셀씩 왼쪽위로 이동하여 가우시안 시작 지점 정하기
//    float2 PixelUvCenter = _Input.Texcoord.xy;
//    float2 StartUV = _Input.Texcoord.xy + (-PixelSize * 3.0f);
//    float2 CurUV = StartUV;
//    float4 ResultColor = (float4) 0.0f;
 
//    // 7×7 샘플링 & 가중치 누적
//    for (int y = 0; y < 7; ++y)
//    {
//        for (int x = 0; x < 7; ++x)
//        {
//            ResultColor += DiffuseTex.Sample(Sampler, CurUV.xy) * Gau7[y][x];
//            CurUV.x += PixelSize.x;
//        }
        
//        CurUV.x = StartUV.x;
//        CurUV.y += PixelSize.y;
//    }
    
//    // 가중치 총합 256 으로 정규화
//    // 이렇게 해야 모든 샘플 값이 커널 가중치만큼 더해져서 전체 이미지가 지나치게 밝아지거나 어두워지는 것 방지
//    ResultColor /= 4096.0f;
       
//    return ResultColor;
//}

// 9x9 가우시안 블러
float4 Blur_PS(PSInput _Input) : SV_TARGET
{
    // 픽셀 하나 사이즈는 몇입니까
    float2 PixelSize = float2(1.0f / ScreenSize.x, 1.0f / ScreenSize.y);
    
    // UV 중심에서 2픽셀씩 왼쪽위로 이동하여 가우시안 시작 지점 정하기
    float2 PixelUvCenter = _Input.Texcoord.xy;
    float2 StartUV = _Input.Texcoord.xy + (-PixelSize * 4.0f);
    float2 CurUV = StartUV;
    float4 ResultColor = (float4) 0.0f;
 
    // 7×7 샘플링 & 가중치 누적
    for (int y = 0; y < 9; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            ResultColor += DiffuseTex.Sample(Sampler, CurUV.xy) * Gau9[y][x];
            CurUV.x += PixelSize.x;
        }
        
        CurUV.x = StartUV.x;
        CurUV.y += PixelSize.y;
    }
    
    // 가중치 총합 256 으로 정규화
    // 이렇게 해야 모든 샘플 값이 커널 가중치만큼 더해져서 전체 이미지가 지나치게 밝아지거나 어두워지는 것 방지
    ResultColor /= 65536.0f;
    
    return ResultColor;
}