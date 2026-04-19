---
title: 그림자 매핑 (Shadow Mapping)
original_url: https://umtimos.tistory.com/207
original_date: 2025-06-12
migrated: 2026-04-19
category: Framework
---

# 그림자 매핑

라이팅이 들어왔어도 그림자가 없으면 캐릭터와 바닥이 붙어 있는지 떠 있는지 구분이 안 간다. 그림자 매핑은 **"빛의 시점에서 무엇이 보이는가"** 를 한 번 그려두고, 그 정보를 카메라 시점에서 참조하는 기법. 디퍼드 파이프라인 위에서 자연스럽게 추가된다.

## 원리 한 문장

빛에서 본 depth 맵을 만들고, 카메라 PS에서 "지금 보고 있는 픽셀이 빛에서 보이나?"를 비교. 안 보이면 그림자.

## 3단계 흐름

1. **Shadow Map 생성** — 빛의 View/Projection으로 오브젝트를 한 번 더 그려서 depth만 기록
2. **Light Pass에서 비교** — 카메라에서 보이는 픽셀을 빛의 클립 공간으로 옮겨 저장된 depth와 비교
3. **Merge 단계에서 음영 반영** — 그림자로 판정된 곳은 diffuse/specular 감소

## Shadow Render Target 준비

라이트마다 전용 RT를 갖는다:

```cpp
Ext_Light::Ext_Light()
{
    LTData = std::make_shared<LightData>();
    LTData->ShadowTargetSizeX = 4096;   // 고해상도
    LTData->ShadowTargetSizeY = 4096;
}

void Ext_Light::Start()
{
    ShadowRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(
        DXGI_FORMAT_R32_FLOAT,
        { LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY },
        float4::RED);      // 초기 색 (빨강) = 아직 아무도 안 씌운 곳
}
```

포맷은 `R32_FLOAT` — depth를 그냥 **픽셀 컬러의 R 채널에 기록**하는 방식(깊이 텍스처 아님). Sampler 비교 기능 안 써도 되는 대신 정밀도는 f32. 프로젝트 규모에선 충분.

## Shadow Pass 셰이더

### VS — 빛 시점 WVP
```hlsl
VSOutput Shadow_VS(VSInput In)
{
    VSOutput Out;
    Out.Position = mul(In.Position, WorldViewProjectionMatrix); // 단, VP는 라이트 기준
    return Out;
}
```

프레임워크가 `WorldViewProjectionMatrix` 안의 View/Projection을 **라이트 기준** 으로 교체해 보낸다.

### PS — 정규화된 depth 기록
```hlsl
float4 Shadow_PS(PSInput In) : SV_TARGET
{
    return float4(max(0.0f, In.Position.z / In.Position.w), 0.0f, 0.0f, 1.0f);
}
```

`z/w`가 [0,1] 범위의 깊이. R 채널에 기록.

### Blend = MIN

여러 오브젝트가 같은 픽셀에 그려지면 **가장 앞의 것**(가장 작은 depth)만 남겨야 한다. D3D11의 `BLEND_OP_MIN`으로 해결:

```cpp
BlendInfo.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
```

덕분에 오브젝트 순서에 관계없이 최종 결과가 가장 앞의 depth 맵이 된다.

## Light Pass에서 그림자 비교

디퍼드 라이팅 PS에서, 카메라로 본 월드 좌표를 **빛의 클립 공간으로 다시 투영**하고 Shadow Map과 비교:

```hlsl
float4 LightPos      = mul(float4(WorldPos.xyz, 1.0f),
                           LTData.LightViewProjectionMatrix);
float3 LightProjection = LightPos.xyz / LightPos.w;  // W-Divide

float2 ShadowUV = float2(
    LightProjection.x * 0.5f + 0.5f,
    LightProjection.y * -0.5f + 0.5f     // DirectX Y 반전
);

float fShadowDepth = ShadowTex.Sample(Sampler, ShadowUV).r;

if (fShadowDepth >= 0.0f &&
    LightProjection.z >= (fShadowDepth + 0.001f))   // ← Bias
{
    OutPut.ShadowTarget.x = 1.0f; // 그림자 영역 마킹
}
```

### Bias의 역할

`0.001f`는 **self-shadowing 완화**를 위한 오프셋. 부동소수 정밀도 때문에 지면이 자기 자신의 그림자를 드리우는 이상한 현상(쉐도우 아크니)을 막는다. 값이 너무 작으면 아크니, 너무 크면 **peter-panning**(그림자가 발에서 떨어져 보임) — 트레이드오프.

## Merge 단계에서 음영 적용

Light pass 결과물의 ShadowTarget 을 Merge에서 활용:

```hlsl
float ShadowMask = ShadowTex.Sample(Sampler, UV).x;

float ShadowStrength = 0.7;    // 얼마나 어둡게 할지
float ShadowFactor = lerp(1.0, 1.0 - ShadowStrength, ShadowMask);
// ShadowMask=0 → Factor=1 (영향 없음)
// ShadowMask=1 → Factor=0.3 (70% 어두움)

DiffuseRatio  *= ShadowFactor;
SpacularRatio *= ShadowFactor;
```

Ambient는 안 건드림 — 그늘진 곳에도 환경광은 있어야 자연스러움.

## 얻은 결과

- 캐릭터 발 밑에 **제대로 된 그림자**가 드리워진다. 바닥에 서 있다는 공간감이 확 살아남.
- Directional Light 기준 그림자가 작동. Point Light 섀도우는 아직 안 함(6면 큐브 섀도우 맵 필요).
- 4096 해상도 덕에 경계가 비교적 선명. 프레임 영향은 체감 가능할 만큼 있음.

## 주의할 점 / 개선거리

- **다이나믹 메시 섀도우** — 현재 구현은 정적 메시 중심. 애니메이션 캐릭터에 대한 Dynamic Shadow는 별도 셰이더(`DynamicShadow_VS`) 경로가 있지만 완전하진 않음. 추후 보강 예정
- **PCF (Percentage Closer Filtering)** — 샘플 1개 비교는 계단 현상이 심하다. 주변 여러 샘플 평균하는 PCF로 부드럽게 가능
- **Cascaded Shadow Map** — 카메라 주변은 촘촘히, 멀리는 러프하게 여러 해상도 섀도우 맵. 광활한 야외 씬에 필수
- **Shadow Bias 동적 조정** — 표면 기울기에 따라 bias를 달리하는 slope-scaled bias
- **Point Light 섀도우** — 큐브맵 depth 6면 필요
- **Front-face culling** — Shadow Pass에서 CULL_FRONT로 바꾸면 peter-panning을 많이 완화
- **MSAA 불호환** — 섀도우 맵은 MSAA가 적용되지 않는 R32_FLOAT 단일 샘플

## 관련 현재 코드
- [DirectX11_Extension/Ext_Light.{h,cpp}](../../DirectX11_Extension/Ext_Light.h)
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) — Shadow Pass 루프
- [Shader/Shadow_VS.hlsl](../../Shader/Shadow_VS.hlsl)
- [Shader/Shadow_PS.hlsl](../../Shader/Shadow_PS.hlsl)
- [Shader/DynamicShadow_VS.hlsl](../../Shader/DynamicShadow_VS.hlsl)
