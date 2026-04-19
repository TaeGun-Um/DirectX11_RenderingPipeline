---
title: 포스트 프로세싱 적용해보기 (Blur / OldFilm / Distortion)
original_url: https://umtimos.tistory.com/210
original_date: 2025-06-21
migrated: 2026-04-19
category: Framework
---

# 포스트 프로세싱 적용해보기

씬 렌더링이 끝났다고 바로 화면에 쏘지 않는다. 요즘 게임은 **완성된 이미지 한 장을 다시 건드려** 블룸·색보정·DOF·HDR 톤매핑·FXAA 같은 효과를 올린다. 이게 포스트 프로세싱이다. 이미 RT에 그려진 결과를 풀스크린 쿼드로 한 번 더 그려 필터를 입히는 방식.

## 구조

RenderTarget이 효과 리스트를 들고, 매 프레임 순서대로 적용:

```cpp
class Ext_PostProcess { /* 인터페이스 */ };

class Ext_DirectXRenderTarget
{
    template<typename PostType>
    std::shared_ptr<PostType> CreateEffect()
    {
        auto New = std::make_shared<PostType>();
        PostProcessInitialize(New);
        PostProcesses.push_back(New);
        return New;
    }

    void PostProcessing(std::shared_ptr<Ext_Camera> _Camera, float _dt)
    {
        for (auto& p : PostProcesses) p->Apply(_Camera, _dt, this);
    }

    std::vector<std::shared_ptr<Ext_PostProcess>> PostProcesses;
};
```

효과는 상속해서 `Apply()`만 구현. 활성화는 한 줄:

```cpp
CameraRenderTarget->CreateEffect<Ext_Blur>();
CameraRenderTarget->CreateEffect<Ext_OldFilm>();
CameraRenderTarget->CreateEffect<Ext_Distortion>();
```

## 효과 1 — Blur (Gaussian)

화면을 흐리게. 가우시안 커널로 주변 픽셀 가중 평균:

```hlsl
static const float Gau9[9][9] = { /* Pascal row 8, 합 65536 */ };

float4 Blur_PS(PSInput In) : SV_TARGET
{
    float2 PixelSize = 1.0f / ScreenSize.xy;
    float2 Start     = In.Texcoord - PixelSize * 4.0f;
    float4 Result    = 0;

    for (int y = 0; y < 9; ++y)
        for (int x = 0; x < 9; ++x)
        {
            float2 UV = Start + float2(x * PixelSize.x, y * PixelSize.y);
            Result += DiffuseTex.Sample(Sampler, UV) * Gau9[y][x];
        }

    return Result / 65536.0f;
}
```

### 9×9가 비싼 이유, 그리고 해법

9×9 = 81 샘플 × 픽셀 수. HD 해상도면 초당 수억 회 샘플. 실제 엔진은:
1. **해상도 다운샘플** — 먼저 1/2 또는 1/4 크기로 블러하면 샘플링 수가 1/4~1/16
2. **분리형 (Separable)** — 9×9를 9+9로 분해해 수평 블러 → 수직 블러 두 패스. 81 → 18
3. 다시 업스케일

본 프로젝트는 교육용으로 풀 9×9. 이후 최적화 여지 많음.

## 효과 2 — OldFilm

오래된 필름처럼 지직거리는 연출. **시간에 따라 순환하는 여러 노이즈 텍스처**를 0.1초마다 교체:

```cpp
// Ext_OldFilm.cpp (개념)
AccTime += _dt;
if (AccTime >= 0.1f)
{
    CurrentFrameIndex = (CurrentFrameIndex + 1) % NoiseFrameCount;
    AccTime = 0;
}

// PS에서 NoiseTex[CurrentFrameIndex]를 샘플링해 컬러에 섞음
```

셰이더에서는 (개념):
```hlsl
float4 Frame = DiffuseTex.Sample(Sampler, UV);
float3 NoiseCol = NoiseTex.Sample(Sampler, UV).rgb;
Frame.rgb = lerp(Frame.rgb, NoiseCol, 0.3);  // 노이즈 오버레이
return Frame;
```

## 효과 3 — Distortion (물결)

UV 좌표 자체를 흔들어 이미지를 왜곡:

```hlsl
float2 UV = In.Texcoord;
UV.x += sin(UV.y * Frequency + AccTime) / Amplitude;
return DiffuseTex.Sample(Sampler, UV);
```

Frequency 높이면 파장 짧아짐, Amplitude 높이면 흔들림 작아짐. `AccTime`으로 시간에 따라 흐르는 물결 효과.

## 공통 파이프라인

세 효과 다:
- 블렌드: `BaseBlend`
- Depth: depth 테스트 끔 (풀스크린 쿼드)
- Rasterizer: `NonCullingRasterizer`
- 입력 정점: **FullRect** (스크린 전체를 덮는 사각형)

차이점은 PS뿐. "입력 RT 샘플링 → 가공 → 출력 RT" 의 **Ping-Pong** 구조.

## 얻은 결과

- **카메라 RT 위에 아무 효과나 스택처럼 쌓을 수 있는 구조** 완성. `CreateEffect<Ext_Blur>()` 한 줄이면 끝.
- Blur·Distortion·OldFilm 세 기본 효과로 포스트 프로세싱의 기본 패턴 확보.
- 이후 Bloom·DOF·Tonemap 같은 고급 효과도 같은 패턴으로 추가 가능.

## 주의할 점 / 개선거리

- **더블 블렌드·SRV/RTV 동시 바인딩 이슈** — 같은 텍스처를 PS 입력으로 읽으면서 RT로 쓰면 D3D가 경고. Ping-Pong 버퍼로 분리해야 하는데 현재 단순 구현은 주의 필요
- **Blur 최적화 미적용** — 위에 언급한 다운샘플/분리형 기법 도입 시 체감 크게 빨라짐
- **Bloom** — HDR 환경에서 자연스러운 효과. 현재는 LDR 파이프라인이라 구현 전 HDR 스왑체인 필요
- **DOF / 모션 블러** — G-Buffer의 Position/Depth 활용 시 어렵지 않음
- **Tonemap** — HDR→LDR. Reinhard, ACES 같은 표준 연산자 고려
- **FXAA/TAA** — 안티 얼라이어싱은 디퍼드 환경에선 이 단계에서 할 수밖에 없음

## 관련 현재 코드
- [DirectX11_Extension/Ext_PostProcess.{h,cpp}](../../DirectX11_Extension/Ext_PostProcess.h)
- [DirectX11_Extension/Ext_Blur.{h,cpp}](../../DirectX11_Extension/Ext_Blur.h)
- [DirectX11_Extension/Ext_OldFilm.{h,cpp}](../../DirectX11_Extension/Ext_OldFilm.h)
- [DirectX11_Extension/Ext_Distortion.{h,cpp}](../../DirectX11_Extension/Ext_Distortion.h)
- [Shader/Blur_PS.hlsl](../../Shader/Blur_PS.hlsl)
- [Shader/OldFilm_PS.hlsl](../../Shader/OldFilm_PS.hlsl)
- [Shader/Distortion_PS.hlsl](../../Shader/Distortion_PS.hlsl)
