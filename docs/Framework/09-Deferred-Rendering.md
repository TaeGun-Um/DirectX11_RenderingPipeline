---
title: 디퍼드 렌더링으로 변경하기
original_url: https://umtimos.tistory.com/205
original_date: 2025-06-11
migrated: 2026-04-19
category: Framework
---

# 디퍼드 렌더링으로 변경하기

Forward 렌더링으로 라이트 여러 개를 붙이다 보면 곧 한계가 온다. **모든 오브젝트의 모든 픽셀에서 모든 라이트를 계산**하니까 비용이 `오브젝트 × 픽셀 × 라이트` 로 폭증한다. 그림자·포스트 프로세스까지 붙이면 더 악화된다.

해결책이 **디퍼드 렌더링(Deferred Rendering)**. 화면 공간에서 라이팅을 한 번만 계산하도록 재구성한다.

## 디퍼드가 뭔가

두 단계로 분리:

1. **Geometry Pass** — 각 오브젝트의 기하 정보(색·위치·노말·재질)를 여러 장의 **G-Buffer 텍스처**에 기록
2. **Light Pass** — G-Buffer를 픽셀 단위로 읽어 **스크린 공간에서** 라이팅 계산

픽셀 하나를 여러 번 그리지 않기 때문에 라이트 수가 늘어도 픽셀 셰이더 비용이 크게 안 커진다.

## 재구성된 파이프라인

```
1. Geometry Pass     → G-Buffer (MRT)
2. Shadow Pass       → Shadow Depth Maps (라이트 시점)
3. Light Pass        → 라이트별 Diffuse/Specular/Ambient/Shadow 누적
4. Merge             → 위 결과들 합쳐 최종 컬러 생성
5. (Forward 알파 패스) → 반투명 오브젝트 추가
6. PostProcess       → Blur/Distortion/OldFilm 등
7. Present
```

본 프로젝트는 이 흐름이 `Ext_Camera::Rendering` 안에 구현돼 있다.

## G-Buffer 구조

PS가 동시에 여러 RT에 쓰는 **MRT (Multi-Render Target)** 를 쓴다. 각 RT에 "기하 속성 하나씩" 저장:

```hlsl
struct PSOutPut
{
    float4 MeshTarget    : SV_TARGET0;  // 색상 (BaseColor)
    float4 PositionTarget: SV_TARGET1;  // World Position
    float4 NormalTarget  : SV_TARGET2;  // World Normal
    float4 TangentTarget : SV_TARGET3;  // Tangent (PBR 확장용)
    float4 BinormalTarget: SV_TARGET4;  // Binormal
    float4 MaskTarget    : SV_TARGET5;  // 재질 마스크
};
```

C++에서 RT 6개를 같은 MeshRenderTarget 에 묶어 바인딩:
```cpp
MeshRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(
    DXGI_FORMAT_R32G32B32A32_FLOAT, ScreenSize, ZERONULL);    // 0 Color
MeshRenderTarget->AddNewTexture(/* same format */);            // 1 Position
MeshRenderTarget->AddNewTexture(/* ... */);                    // 2 Normal
// ...
MeshRenderTarget->SetDepthTexture(CameraRenderTarget->GetDepthTexture());
```

Depth는 카메라 렌더타겟과 **공유** 한다. 이후 알파 패스에서도 이걸 재활용하기 위함 ([알파 패스 문서](12-Transparency-Test.md)).

## Light Pass — 풀스크린 쿼드로 누적

라이트별로 풀스크린 쿼드를 그리고, 그 PS에서 G-Buffer를 샘플링해 라이팅을 계산한다:

```hlsl
Texture2D PositionTex : register(t0);
Texture2D NormalTex   : register(t1);
Texture2D ShadowTex   : register(t2);

PSOutput DeferredLight_PS(PSInput In) : SV_TARGET
{
    float3 WorldPos  = PositionTex.Sample(Sampler, In.Texcoord).xyz;
    float3 WorldNorm = NormalTex.Sample  (Sampler, In.Texcoord).xyz;

    LightData L = Lights[0]; // 라이트 하나씩 풀스크린 드로우
    // ... 기존 Phong 계산과 동일하지만 입력이 G-Buffer에서 옴
}
```

이 패스의 블렌드는 **`OneBlend` (ONE + ONE + ADD)** — 라이트별 결과가 화면에 누적된다.

## OM State 이슈 — Depth ALWAYS

라이트 패스의 풀스크린 쿼드는 depth 비교가 의미 없으므로 `AlwayDepth` (DepthFunc=ALWAYS)를 쓴다. 하지만 이 상태가 `DepthWriteMask=ALL` 이면 **opaque 깊이를 덮어쓰는** 버그가 생긴다 → 반투명 패스가 깨짐.

이 문제를 나중에 [반투명 테스트](12-Transparency-Test.md)에서 고치며 **`MergeDepth` (쓰기 OFF)** 를 새로 만들어 분리했다.

## Merge 단계

라이트별로 누적된 결과물을 **하나의 최종 컬러**로 합치는 풀스크린 쿼드:

```hlsl
// DeferredMerge_PS.hlsl 중 일부
float4 Albedo          = BaseColorTex.Sample(Sampler, UV);
float4 DiffuseRatio    = DiffuseTex .Sample(Sampler, UV);
float4 SpacularRatio   = SpecularTex.Sample(Sampler, UV);
float4 AmbientRatio    = AmbientTex .Sample(Sampler, UV);
float  ShadowMask      = ShadowTex  .Sample(Sampler, UV).x;

float ShadowFactor = lerp(1.0, 1.0 - 0.7, ShadowMask);
DiffuseRatio  *= ShadowFactor;
SpacularRatio *= ShadowFactor;

Out.Color.rgb = Albedo.rgb * (DiffuseRatio.rgb + SpacularRatio.rgb + AmbientRatio.rgb);
Out.Color.a   = 1.0f;
```

블렌드는 **`MergeBlend`** (`SrcColor + DestColor * (1 - SrcAlpha)`) — 투명도 있는 SkyBox·이펙트와도 자연스럽게 합성됨.

## 얻은 결과

- **라이트 수가 늘어도 프레임 성능이 거의 그대로**. 라이트별 비용은 풀스크린 쿼드 한 장.
- Shadow·포스트 프로세스 같은 후속 기능을 **화면 공간**에서 돌릴 수 있는 기반이 깔림 — G-Buffer에서 Position/Normal을 꺼내 쓰면 됨.
- 렌더 타겟·블렌드·Depth 규칙이 명확해져 이후 확장이 쉬워짐.

## 주의한 트레이드오프 / 개선거리

- **반투명 처리가 까다로워짐** — G-Buffer는 픽셀당 하나만 저장. 여러 반투명이 겹치면 이 구조만으론 불가능. 별도 Forward 알파 패스 필요 → [알파 패스](12-Transparency-Test.md)에서 해결
- **MSAA 호환성** — 디퍼드는 MSAA가 까다로움. 요즘은 TAA/FXAA로 대체하는 추세
- **VRAM 사용량 증가** — G-Buffer 여러 장이 항상 메모리에 상주. 포맷 최적화(`R16G16B16A16_FLOAT` 등)로 줄일 수 있음
- **재질 다양성** — G-Buffer에 담기는 정보가 고정이면 특수 셰이딩(토이 셰이더, 피부 SSS 등)이 어렵다. Material ID 슬롯을 둬 분기 가능
- **라이트 컬링** — 현재는 모든 라이트가 풀스크린으로 계산. Point Light는 영향권 안만 계산하도록 **Light Volume Pass** 필요

## 다음 단계

- [그림자 매핑](10-Shadow-Mapping.md) — 디퍼드 위에서 깔끔하게 올라감
- [포스트 프로세싱](11-PostProcess.md) — 풀스크린 효과들
- [반투명 Forward 패스](12-Transparency-Test.md) — 디퍼드의 한계를 Forward로 보완
- [디퍼드 알파 패스 재정비 (2026-04)](../WorkLog/2026-04-19.md#5-디퍼드-알파-패스-정리) — Depth/Merge state 버그 수정 이력

## 관련 현재 코드
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) `Rendering()` — 전체 디퍼드 루프
- [Shader/DeferredLight_PS.hlsl](../../Shader/DeferredLight_PS.hlsl)
- [Shader/DeferredMerge_PS.hlsl](../../Shader/DeferredMerge_PS.hlsl)
- [Shader/LightData.fx](../../Shader/LightData.fx)
