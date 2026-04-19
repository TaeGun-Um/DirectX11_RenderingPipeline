---
title: Texcoord(UV)와 Sampler
original_url: https://umtimos.tistory.com/195
original_date: 2025-05-28
migrated: 2026-04-19
category: DirectX
---

# Texcoord(UV)와 Sampler

텍스처를 로드했다고 끝이 아니다. "이 픽셀은 텍스처의 어느 지점 색을 가져올까?"를 결정하는 **UV**와, "그 지점을 어떻게 샘플링할까?"를 정하는 **Sampler** 설정이 필요하다.

## UV란

`TEXCOORD`는 HLSL 시맨틱으로 텍스처 좌표를 넘길 때 쓴다. 보통 `float2`, 범위는 `[0, 1]`:

| UV | 텍스처 위치 |
|---|---|
| (0, 0) | 좌상단 |
| (1, 0) | 우상단 |
| (0, 1) | 좌하단 |
| (1, 1) | 우하단 |

같은 2D 텍스처를 씌워도 UV를 어떻게 펼쳐주느냐에 따라 타일링·스크롤·왜곡이 전부 달라진다.

### 쓰임새
- **텍스처 매핑** — 각 픽셀이 텍스처의 어느 영역에 대응되는지
- **애니메이션** — UV에 시간을 더해 스크롤/웨이브
- **주소 처리** — 0~1 범위를 벗어난 UV를 어떻게 처리할지는 Sampler가 결정

## UV 값이 실제로 어떻게 작동하는지 확인

정육면체 6면 중 뒤(-Z) 한 면에만 UV를 부여하고, PS에서 `UV.x > 0.5` 면 노랑을 반환하도록 해 보면 어느 면에 매핑됐는지 한눈에 보인다:

```cpp
// 뒤(-Z) 면에만 UV 부여
Vertex[4].TEXCOORD = { 0.0f, 0.0f };
Vertex[5].TEXCOORD = { 1.0f, 0.0f };
Vertex[6].TEXCOORD = { 1.0f, 1.0f };
Vertex[7].TEXCOORD = { 0.0f, 1.0f };
// 나머지 면은 {0, 0}
```

```hlsl
if (In.TexCoord.x > 0.5) return float4(1, 1, 0, 1);
else                     return DefaultTex.Sample(Sampler, In.TexCoord);
```

원하는 면에서만 노란 절반이 보이는 걸 확인하면 UV가 제대로 적용된 것.

## Sampler State

Sampler는 "텍스처 좌표를 **어떻게** 해석하고 샘플링할 것인지"를 정의한다. 주로 세 가지를 설정:

1. **Filter** — Point / Linear / Anisotropic
2. **Address Mode** (U/V/W축) — Wrap / Clamp / Mirror / Border
3. **밉맵과 Anisotropic 레벨**

### 예: Linear + Clamp

```cpp
D3D11_SAMPLER_DESC Info = { 0 };
Info.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   // 부드러운 보간
Info.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
Info.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
Info.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
Info.ComparisonFunc = D3D11_COMPARISON_NEVER;
Info.MinLOD         = 0.0f;
Info.MaxLOD         = D3D11_FLOAT32_MAX;

Ext_DirectXSampler::CreateSampler("LinearClampSampler", Info);
```

HLSL에서:
```hlsl
Texture2D    DiffuseTex : register(t0);
SamplerState Sampler    : register(s0);

float4 PS(PSInput In) : SV_TARGET
{
    return DiffuseTex.Sample(Sampler, In.TexCoord);
}
```

## Filter 종류

| 필터 | 특성 |
|---|---|
| `MIN_MAG_MIP_POINT` | 픽셀 느낌, 계단이 확 보임 (레트로 스타일) |
| `MIN_MAG_MIP_LINEAR` | 부드러움, 대부분의 경우 기본값 |
| `ANISOTROPIC` | 경사진 바닥 같은 곳에서 특히 선명 (AnisoLevel 1~16) |

## Address Mode 동작

UV를 `-0.5 ~ 1.5` 범위로 테스트하면 차이가 확연하다:

| Mode | 동작 | 쓰임 |
|---|---|---|
| `WRAP` | 타일링, 0~1 경계를 넘으면 반복 | 바닥·벽 반복 텍스처 |
| `CLAMP` | 경계 픽셀 색을 그대로 연장 | UI, 스카이박스 |
| `MIRROR` | 0~1 왕복 반복, 방향 교대로 뒤집힘 | 좌우 대칭 타일링 |
| `BORDER` | 경계 밖은 `BorderColor`로 채움 | 그림자 맵 경계 처리 |

## 프로젝트에 등록된 Sampler

[Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) `MakeSampler()`에서 다음을 등록:

- `LinearClampSampler` — UI/스카이박스/G-Buffer 읽기
- `LinearWrapSampler` — 타일링 텍스처
- `PointClampSampler` — 디퍼드 라이트 합성 (정확한 픽셀 매칭)
- `PointWrapSampler`, `LinearMirror`, `PointMirror` — 특수 용도
- `CubeMapSampler` — 큐브맵 전용 설정
- `ShadowCompare` — 섀도우 맵용 `SamplerComparisonState`

## 얻은 결과

- 텍스처가 **어떻게 보일지**를 런타임에서 유연하게 바꿀 수 있게 됨. 같은 텍스처도 필터/Address만 바꿔 다른 분위기 연출 가능.
- 타일링, 미러링, 경계 처리 같은 흔한 요구가 별도 셰이더 코드 없이 해결.
- 섀도우 맵(`SamplerComparisonState`)처럼 특수한 비교 샘플러도 같은 인프라로 관리됨.

## 다음 단계 / 개선거리

- **Anisotropic 도입** — 현재는 전부 LINEAR. 경사진 표면(바닥/벽)에서 Aniso 4~8로 선명도 크게 개선 가능
- **LOD Bias 튜닝** — 밉맵 사용 시 `MipLODBias`로 전체적인 선명도 밸런스
- **텍스처 품질 프리셋** — Low/Medium/High 단계별 필터/Aniso 구성을 Config에 넣고 런타임 전환
- **Shadow Sampler 분리** — 현재 `ShadowCompare` 하나로 처리하지만, PCF 탭 수나 비교 함수를 조절할 여지가 있음

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXSampler.{h,cpp}](../../DirectX11_Extension/Ext_DirectXSampler.h)
- [DirectX11_Extension/Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) `MakeSampler()`
