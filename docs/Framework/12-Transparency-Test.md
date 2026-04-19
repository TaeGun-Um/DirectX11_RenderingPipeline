---
title: 반투명 물체 테스트하기 (디퍼드 위에서의 알파)
original_url: https://umtimos.tistory.com/211
original_date: 2025-06-21
migrated: 2026-04-19
category: Framework
---

# 반투명 물체 테스트하기

디퍼드 렌더링으로 파이프라인을 옮긴 뒤, **반투명 오브젝트**를 넣어봤는데 예상대로 "그냥 안 되는" 상태였다. 왜 안 되는지, 그리고 어떻게 우회하는지가 이 글의 주제.

## 디퍼드와 반투명의 근본 충돌

디퍼드의 핵심은 **"한 픽셀 = 하나의 G-Buffer 엔트리"**. 그런데 반투명은 원리상 **여러 장이 겹쳐 보이는** 상황이 자연스럽다. 유리 뒤에 또 유리, 그 뒤에 벽이 있는 장면을 G-Buffer에 담는 건 픽셀당 하나의 노말/컬러/위치만 저장되므로 **표현 불가능**.

| 디퍼드 전제 | 반투명 요구 |
|---|---|
| 픽셀 = 가장 앞의 오브젝트 1개 | 픽셀 = 여러 레이어의 합 |
| 라이팅 = G-Buffer에서 1회 | 라이팅 = 레이어별 |
| 불투명 정렬 불필요 | back-to-front 정렬 필수 |

## 첫 시도와 발견된 문제

블로그 당시엔 `StaticAlpha` 머티리얼 + 별도 `AlphaRenderTarget`을 만들어 "알파 패스 = AlphaRT에 그림 → 나중에 Merge" 구조를 시도했다. 하지만 확인해보니 문제가 여러 겹으로 있었다:

1. **AlphaRT에 depth 미첨부** — 불투명 뒤에 가려져 있어도 앞에 그려짐 (캐릭터 뚫고 보임)
2. **StaticAlpha가 `EngineDepth`(쓰기 ON)** — 알파끼리 서로 간섭
3. **AlphaMerge 단계의 더블 블렌드** — 블렌드가 두 번 적용돼 50% 의도가 25% 가 됨
4. **Merge 머티리얼들이 `AlwayDepth`(WriteMask=ALL)** — 합성 풀스크린 쿼드가 opaque depth를 싹 덮어씀

4번이 가장 치명적이었다. "왜 알파 오브젝트의 depth 테스트가 엉망이지?" 했더니, 그 이전 Merge 단계에서 **depth 버퍼가 이미 덮어써진 상태**였던 것.

## 재구성 — 디퍼드 + Forward 알파 패스

표준 해결책은 "**불투명은 디퍼드, 반투명은 Forward**". 한 프레임 안에서 두 패스를 섞어 쓴다:

1. Opaque 디퍼드 패스 (G-Buffer 생성)
2. Shadow, Light, LightMerge (기존 그대로)
3. SkyBox + LightMerge 를 CameraRT에 합성 (여기서 **depth는 건드리지 않음**)
4. **Forward 알파 패스** — CameraRT에 직접 그림, opaque depth 공유, BaseBlend, Depth 쓰기 OFF
5. PostProcess
6. Present

### 새로 만든 Depth/State

| 이름 | 목적 | 설정 |
|---|---|---|
| `AlphaDepth` | 반투명 Forward | DepthEnable ON, Write OFF, Func LESS_EQUAL |
| `MergeDepth` | Merge 풀스크린 쿼드 | DepthEnable OFF — depth 안 건드림 |

### Depth 보존을 위한 `RenderTargetColorClear()`

기존 `RenderTargetClear()` 는 color + depth 둘 다 clear. 알파 패스 직전엔 depth를 유지해야 하므로 **색만 clear**하는 헬퍼 추가:

```cpp
void RenderTargetColorClear()
{
    RenderTargetViewsClear();   // color만
    // DepthStencilViewClear()를 호출하지 않음
}
```

### 알파 패스 코드 (Ext_Camera)

```cpp
// 합성 직전: 컬러만 clear, depth는 opaque에서 쓴 그대로
CameraRenderTarget->RenderTargetColorClear();
CameraRenderTarget->Merge(SkyBoxRenderTarget);
CameraRenderTarget->Merge(LightMergeRenderTarget);

// ==== Forward 알파 패스 ====
// 라이팅까지 완료된 CameraRT에 직접 그림
// AllRenderUnits는 far→near 정렬 상태
CameraRenderTarget->RenderTargetSetting();
for (auto& Unit : AllRenderUnits)
{
    auto Owner = Unit->GetOwnerMeshComponent().lock();
    if (!Owner || !Unit->GetIsAlpha()) continue;

    if (UpdatedComponents.insert(Owner).second)
        Owner->Rendering(_dt, CamPos, View, Proj);

    Unit->Rendering(_dt);  // StaticAlpha 머티리얼: AlphaDepth + BaseBlend
}

CameraRenderTarget->PostProcessing(...);
```

기존 `AlphaMerge(AlphaRenderTarget)` 호출은 삭제 (더블 블렌드 원인).

## 얻은 결과

- 반투명 오브젝트가 **opaque 뒤에 있으면 제대로 가려짐**. 캐릭터 뒤에 있는 유리창은 캐릭터 몸에 의해 정확히 컬.
- **back-to-front 정렬**로 여러 반투명이 겹쳐도 자연스럽게 블렌드.
- 더블 블렌드 제거 → 50% 의도하면 정확히 50% 로 합성됨.
- 기존 구조를 거의 해치지 않으면서 **디퍼드 + 포워드 하이브리드** 를 완성.

## 다음 단계 / 개선거리

- **반투명 오브젝트의 라이팅 일관성** — 현재 `FGraphics_PS`가 Forward 라이팅을 직접 돌림. 디퍼드 opaque와 결과가 약간 다를 수 있음. 동일한 LightData cbuffer를 쓰게끔 맞춤
- **Order-Independent Transparency (OIT)** — back-to-front 정렬이 항상 옳은 건 아님(파티클 클라우드 같은 복잡한 경우). Weighted Blended OIT, Depth Peeling 등 기법 고려
- **그림자를 받는 반투명** — 현재 알파 오브젝트는 그림자 드리움 X, 받음 X. ShadowTex 샘플링 추가 필요
- **포스트 프로세스 포함 여부** — 알파 패스는 CameraRT에 직접 들어가므로 Blur 등에 자동 영향. 특정 이펙트를 알파 이후에만 하고 싶을 수도 있음

## 히스토리

- **2025-06-21**: 블로그 원글 — 문제 제기 + 별도 AlphaRT + AlphaMerge 시도
- **2026-04-19**: [리팩토링 세션](../WorkLog/2026-04-19.md#5-디퍼드-알파-패스-정리)에서 현재 형태(Forward 알파 패스)로 재구성

## 관련 현재 코드
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) — `Rendering()` 내 알파 패스
- [DirectX11_Extension/Ext_DirectXRenderTarget.{h,cpp}](../../DirectX11_Extension/Ext_DirectXRenderTarget.h) — `RenderTargetColorClear`
- [DirectX11_Extension/Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) — `AlphaDepth`, `MergeDepth`, `StaticAlpha` 정의
- [Shader/FGraphics_PS.hlsl](../../Shader/FGraphics_PS.hlsl) — Forward 알파용 PS
- [DirectX11_Contents/RectActor.cpp](../../DirectX11_Contents/RectActor.cpp) / [SphereActor.cpp](../../DirectX11_Contents/SphereActor.cpp) — 반투명 테스트 액터
