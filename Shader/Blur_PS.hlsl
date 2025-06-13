// ����þ� Ŀ��
// Total = 16+64+96+64+16 = 256
static float Gau[5][5] =
{
    { 1, 4, 6, 4, 1 },
    { 4, 16, 24, 16, 4 },
    { 6, 24, 36, 24, 6 },
    { 4, 16, 24, 16, 4 },
    { 1, 4, 6, 4, 1 }
};

// 7��7 ����þ� Ŀ�� (Pascal row 6: 1, 6, 15, 20, 15, 6, 1)
// ��ü ����ġ �� = (1+6+15+20+15+6+1)�� = 64�� = 4096
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

// 9��9 ����þ� Ŀ�� (Pascal row 8: 1, 8, 28, 56, 70, 56, 28, 8, 1)
// ��ü ����ġ �� = (1+8+28+56+70+56+28+8+1)�� = 256�� = 65536
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
    float4 ScreenSize; // ȭ�� ũ��
    float AccTime; // ����� �ð�
};

Texture2D DiffuseTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

// 5x5 ����þ� ��
//float4 Blur_PS(PSInput _Input) : SV_TARGET
//{
//    // �ȼ� �ϳ� ������� ���Դϱ�
//    float2 PixelSize = float2(1.0f / ScreenSize.x, 1.0f / ScreenSize.y);
    
//    // UV �߽ɿ��� 2�ȼ��� �������� �̵��Ͽ� ����þ� ���� ���� ���ϱ�
//    float2 PixelUvCenter = _Input.Texcoord.xy;
//    float2 StartUV = _Input.Texcoord.xy + (-PixelSize * 2.0f);
//    float2 CurUV = StartUV;
//    float4 ResultColor = (float4) 0.0f;
 
//    // 5��5 ���ø� & ����ġ ����
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
    
//    // ����ġ ���� 256 ���� ����ȭ
//    // �̷��� �ؾ� ��� ���� ���� Ŀ�� ����ġ��ŭ �������� ��ü �̹����� ����ġ�� ������ų� ��ο����� �� ����
//    ResultColor /= 256.0f;
    
//    return ResultColor;
//}

// 7x7 ����þ� ��
//float4 Blur_PS(PSInput _Input) : SV_TARGET
//{
//    // �ȼ� �ϳ� ������� ���Դϱ�
//    float2 PixelSize = float2(1.0f / ScreenSize.x, 1.0f / ScreenSize.y);
    
//    // UV �߽ɿ��� 2�ȼ��� �������� �̵��Ͽ� ����þ� ���� ���� ���ϱ�
//    float2 PixelUvCenter = _Input.Texcoord.xy;
//    float2 StartUV = _Input.Texcoord.xy + (-PixelSize * 3.0f);
//    float2 CurUV = StartUV;
//    float4 ResultColor = (float4) 0.0f;
 
//    // 7��7 ���ø� & ����ġ ����
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
    
//    // ����ġ ���� 256 ���� ����ȭ
//    // �̷��� �ؾ� ��� ���� ���� Ŀ�� ����ġ��ŭ �������� ��ü �̹����� ����ġ�� ������ų� ��ο����� �� ����
//    ResultColor /= 4096.0f;
       
//    return ResultColor;
//}

// 9x9 ����þ� ��
float4 Blur_PS(PSInput _Input) : SV_TARGET
{
    // �ȼ� �ϳ� ������� ���Դϱ�
    float2 PixelSize = float2(1.0f / ScreenSize.x, 1.0f / ScreenSize.y);
    
    // UV �߽ɿ��� 2�ȼ��� �������� �̵��Ͽ� ����þ� ���� ���� ���ϱ�
    float2 PixelUvCenter = _Input.Texcoord.xy;
    float2 StartUV = _Input.Texcoord.xy + (-PixelSize * 4.0f);
    float2 CurUV = StartUV;
    float4 ResultColor = (float4) 0.0f;
 
    // 7��7 ���ø� & ����ġ ����
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
    
    // ����ġ ���� 256 ���� ����ȭ
    // �̷��� �ؾ� ��� ���� ���� Ŀ�� ����ġ��ŭ �������� ��ü �̹����� ����ġ�� ������ų� ��ο����� �� ����
    ResultColor /= 65536.0f;
    
    return ResultColor;
}