---
title: BlendState 생성
original_url: https://umtimos.tistory.com/196
original_date: 2025-05-28
migrated: 2026-04-19
category: DirectX
---

# BlendState 생성

## 왜 Blend를 다뤄야 하는가

픽셀 셰이더가 색을 계산해도 그대로 화면에 나가진 않는다. Output Merger 단계에서 **이미 렌더타겟에 있던 색(Dest)** 와 **방금 계산한 색(Src)** 을 어떻게 섞을지 결정해야 하는데, 그 규칙이 `BlendState`다.

투명한 유리창을 그릴 때, 유리 너머의 풍경(Dest)과 유리 자체의 색(Src)을 자연스럽게 섞어야 하는 경우가 대표적. 라이트 누적, UI 오버레이, 글로우 효과 같은 것도 전부 블렌드가 기반이다.

## 기본 공식

```
FinalColor = SrcColor * SrcBlend  +  DestColor * DestBlend
```

어떤 값을 곱할지(`SrcBlend` / `DestBlend`)와 어떻게 합칠지(`BlendOp`: ADD / SUBTRACT / MIN / MAX 등)를 고르는 게 BlendState의 핵심이다.

## 가장 쓰이는 3가지 조합

### 1. Alpha Blend — 반투명 표준
```
Final = SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
```
- `SrcBlend = D3D11_BLEND_SRC_ALPHA`
- `DestBlend = D3D11_BLEND_INV_SRC_ALPHA`
- `BlendOp = D3D11_BLEND_OP_ADD`

50% 투명 유리, UI 위젯 겹침 등. 본 프로젝트의 `BaseBlend`.

### 2. Additive Blend — 광원/글로우
```
Final = SrcColor + DestColor
```
- 둘 다 `ONE`, `ADD`
- 네온, 파티클, 디퍼드 라이트 누적에 씀. 본 프로젝트의 `OneBlend`.

### 3. Min/Max Blend — 그림자/마스크
```
Final = min(Src, Dest)  또는  max(Src, Dest)
```
- `BlendOp = D3D11_BLEND_OP_MIN / MAX`
- 그림자 마스크 누적에 씀. 본 프로젝트의 `MinBlend`.

## 기본 BlendState (블렌드 OFF)

`BlendEnable = FALSE` 면 결과는 그냥 Src가 Dest를 덮어쓴다(불투명 기본 동작):

```cpp
D3D11_BLEND_DESC Desc = { 0 };
Desc.AlphaToCoverageEnable  = FALSE;
Desc.IndependentBlendEnable = FALSE;

auto& rt = Desc.RenderTarget[0];
rt.BlendEnable = FALSE;
rt.SrcBlend    = D3D11_BLEND_ONE;
rt.DestBlend   = D3D11_BLEND_ZERO;
rt.BlendOp     = D3D11_BLEND_OP_ADD;
```

## Alpha Blend State 생성 예

```cpp
D3D11_BLEND_DESC Info = { 0 };
Info.AlphaToCoverageEnable  = false;
Info.IndependentBlendEnable = false;

auto& rt = Info.RenderTarget[0];
rt.BlendEnable           = true;
rt.SrcBlend              = D3D11_BLEND_SRC_ALPHA;
rt.DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
rt.BlendOp               = D3D11_BLEND_OP_ADD;
rt.SrcBlendAlpha         = D3D11_BLEND_ONE;
rt.DestBlendAlpha        = D3D11_BLEND_ONE;
rt.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

Ext_DirectXBlend::CreateBlend("BaseBlend", Info);
```

## 각 필드 요약

| 필드 | 의미 |
|---|---|
| `BlendEnable` | 블렌드 사용 여부 |
| `SrcBlend` | Src 컬러(RGB)에 곱할 계수 |
| `DestBlend` | Dest 컬러(RGB)에 곱할 계수 |
| `BlendOp` | RGB 합치는 방법 (ADD/SUB/MIN/MAX) |
| `SrcBlendAlpha` / `DestBlendAlpha` / `BlendOpAlpha` | 알파 채널에 대한 동일 규칙 |
| `RenderTargetWriteMask` | R/G/B/A 중 어떤 채널을 쓸 건지 |
| `IndependentBlendEnable` | MRT 각 RT마다 다른 Blend 쓸지 |

## 얻은 결과

이 단계로 반투명 표현의 기반이 깔렸다. 직후 이어지는 작업:
- [반투명 테스트](../Framework/12-Transparency-Test.md) — 실제로 Rect/Sphere를 반투명으로 그려 디퍼드 파이프라인과 엮을 때 생긴 문제와 해결 과정
- 디퍼드 라이팅의 **라이트 누적**에 `OneBlend` 재활용
- 그림자 마스크 합성에 `MinBlend` 사용

## 다음 단계 / 개선거리
- **Premultiplied Alpha** 도입 — RT에 미리 곱해진 알파를 쓰면 체이닝 시 더블 블렌드 문제가 줄어듦
- **Independent Blend** — MRT 디퍼드에서 RT0은 라이트 누적, RT1은 불투명 처리처럼 분리가 필요해지면 `IndependentBlendEnable = TRUE` 로
- **Premultiplied vs Straight Alpha** 일관된 정책 문서화

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXBlend.{h,cpp}](../../DirectX11_Extension/Ext_DirectXBlend.h)
- [DirectX11_Extension/Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) `MakeBlend()` — BaseBlend / OneBlend / MinBlend / MergeBlend
