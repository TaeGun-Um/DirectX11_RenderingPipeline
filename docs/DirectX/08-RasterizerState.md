---
title: RasterizerState 생성
original_url: https://umtimos.tistory.com/191
original_date: 2025-05-28
migrated: 2026-04-19
category: DirectX
---

# RasterizerState 생성

VS가 정점을 변환해 놓으면, 그 삼각형을 실제 픽셀로 "쪼개는" 단계가 Rasterizer다. 3D 좌표에서 2D 픽셀로 넘어가는 다리 역할을 하는데, 여기서 꽤 많은 일이 한꺼번에 벌어진다.

## Rasterizer가 한 번에 처리하는 것들

1. **Rasterization** — Clip Space → NDC → Screen Space 변환 (W-Divide 포함)
2. **Back-face Culling** — 뒤로 돌아서있는 삼각형 버림
3. **Screen Clipping** — 프러스텀 밖으로 나간 부분 버림
4. **Viewport Transformation** — NDC → 실제 픽셀 좌표
5. **Scan Conversion** — 삼각형 내부에 해당하는 픽셀 식별
6. **Attribute Interpolation** — 정점 속성을 픽셀 단위로 선형 보간 (UV·Normal·Color 전부 여기서 내려옴)

이 중에 설정할 수 있는 게 주로 **Culling**과 **Fill Mode**.

## 기본값

아무 State도 바인딩하지 않으면 이렇게 돌아간다:

| 필드 | 기본값 | 뜻 |
|---|---|---|
| `FillMode` | `D3D11_FILL_SOLID` | 폴리곤 내부 채움 |
| `CullMode` | `D3D11_CULL_BACK` | 뒷면 컬링 ON |
| `FrontCounterClockwise` | `FALSE` | **CW(시계방향)** 정점 순서를 앞면으로 |
| `DepthClipEnable` | `TRUE` | 프러스텀 외부 컬링 |

## 프로젝트 초기에 겪은 함정

이 프로젝트의 사각형 메시는 인덱스 순서가 `{0, 1, 2, 0, 2, 3}` — **CCW(반시계)** 순서다. 근데 기본 Rasterizer는 CW를 앞면으로 보기 때문에, 내가 그린 메시는 전부 **뒷면**으로 인식되어 컬링됐다. 화면에 아무것도 안 나오는 상태가 된 것.

눈치를 못 채면 "어? 왜 아무것도 안 보이지 Clear만 되네" 한참 헤매게 되는 함정. 해결은 간단하다:

## 커스텀 RasterizerState

`FrontCounterClockwise`를 `TRUE`로 바꾸면 CCW가 앞면이 된다:

```cpp
void Ext_DirectXResourceLoader::MakeRasterizer()
{
    D3D11_RASTERIZER_DESC Desc = { 0 };
    Desc.FillMode              = D3D11_FILL_SOLID;
    Desc.CullMode              = D3D11_CULL_BACK;
    Desc.FrontCounterClockwise = TRUE;        // ← 핵심
    Desc.DepthClipEnable       = TRUE;

    Ext_DirectXRasterizer::CreateRasterizer("BasicRasterizer", Desc);
}
```

머티리얼에서 `SetRasterizer("BasicRasterizer")`로 바인딩.

## 프로젝트에 등록된 Rasterizer

| 이름 | CullMode | 용도 |
|---|---|---|
| `BasicRasterizer` | `CULL_BACK` | 일반 메시 |
| `NonCullingRasterizer` | `CULL_NONE` | 스카이박스 / 풀스크린 쿼드 / 양면 메시 |

풀스크린 쿼드(Merge 머티리얼)는 보는 각도가 카메라 뒤로 갈 일도 있어 `CULL_NONE`이 안전하다.

### FillMode 옵션 (미사용)
- `D3D11_FILL_WIREFRAME` — 와이어프레임 디버깅용
- `D3D11_FILL_SOLID` — 기본값

본 프로젝트는 Debug 머티리얼 정도에서만 와이어프레임을 쓸 수 있다.

## 얻은 결과

- **사각형이 드디어 보인다**. 종횡비·회전이 의도대로 적용되고 뒷면 컬링도 정상 동작.
- 양면 렌더링이 필요한 경우(`NonCullingRasterizer`)와 일반 메시(`BasicRasterizer`)를 섞어 쓸 수 있게 됨.

## 주의할 미래 상황

- **뒤집힌 스케일(-1)** 을 쓰면 정점 순서가 뒤집혀 컬링이 엉킨다. 미러/뒤집힌 오브젝트 지원할 때 `CullMode`를 동적으로 바꿔야 할 수 있음.
- **섀도우 맵** 생성 시 `CULL_FRONT`로 바꿔 peter-panning(그림자가 발에서 떨어져 보이는 현상)을 줄일 수 있다. 현재 Shadow 머티리얼은 `NonCullingRasterizer`를 쓰는데, 이 부분은 나중에 튜닝 여지 있음.
- **Scissor Rect** — 특정 화면 영역만 그리고 싶을 때 `ScissorEnable = TRUE`. UI 클리핑이나 뷰포트 분할에 유용.

## 다음 단계 / 개선거리

- 와이어프레임 디버그 토글 — `WireframeRasterizer` 추가해 개발 중 메시 디버깅
- Shadow용 front-face culling 변형 state — peter-panning 완화
- MSAA 도입 시 `MultisampleEnable = TRUE`와 쌍으로 세팅 필요
- 카메라 flycam처럼 뒤집어 보는 모드에서는 CULL 방향 잠시 뒤집기 고려

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXRasterizer.{h,cpp}](../../DirectX11_Extension/Ext_DirectXRasterizer.h)
- [DirectX11_Extension/Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) `MakeRasterizer()`
