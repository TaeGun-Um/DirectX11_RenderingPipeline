---
title: 카메라와 뷰·프로젝션·뷰포트 행렬
original_url: https://umtimos.tistory.com/182
original_date: 2025-05-26
migrated: 2026-04-19
category: DirectX
---

# 카메라와 뷰 · 프로젝션 · 뷰포트 행렬

3D 렌더링에서 가장 직관적이지 않은 부분이 **좌표 공간의 연쇄 변환**이다. 월드 공간에 있는 메시가 최종적으로 화면에 찍히려면 네 단계의 공간을 거친다:

```
World → View(카메라 기준) → Clip(투영) → NDC → Screen(픽셀)
```

View와 Projection 행렬은 이 변환의 "원리"를 제공하고, Viewport는 NDC→픽셀 매핑만 마무리한다. 하나씩 풀어보자.

## 시작: 왜 도형이 찌그러져 보이는가

앞선 InputAssembler 단계에서 정사각형을 그렸더니 가로로 늘어난 직사각형이 됐다. 원인은 단순하다 — **NDC는 사각형이지만 화면은 16:9**.

NDC 좌표 범위:

| 축 | 범위 |
|---|---|
| X | -1.0 ~ 1.0 |
| Y | -1.0 ~ 1.0 |
| Z | 0.0 ~ 1.0 (D3D 기본) |

창이 1280×720이면 NDC의 X 구간(2)이 1280px에, Y 구간(2)이 720px에 매핑되므로 가로 배율이 더 크다. 정사각형이 가로로 늘어난다.

해결: **View → Projection 행렬**을 거쳐 정점을 적절한 비율로 변형한 뒤, 뷰포트가 픽셀로 내려보낸다.

## 카메라라는 개념

View와 Projection을 계산하려면 기준이 필요하고, 그 기준이 **카메라**다. 이 프로젝트에선 `Ext_Camera` 클래스가 `Ext_Actor`를 상속해 위치·회전·스케일을 가진다(초기값 위치 0, 회전 0, 스케일 1).

## 뷰 행렬 (View Matrix)

> 월드에 흩어진 모든 물체를 **카메라 기준 좌표계**로 재배치한다.

카메라가 (0,0,0)에, 캐릭터가 (0,0,100)에 있고 카메라가 30° 회전한 상태라면, 뷰 행렬은 "카메라를 원점·회전 0으로 맞추면서 나머지를 그만큼 반대로 이동/회전시킨다". 결과적으로 카메라 공간에서는 항상 카메라가 원점이다.

### `XMMatrixLookToLH(EyePos, EyeDir, EyeUp)` 내부

```cpp
float4 EyeDir   = _EyeDir.NormalizeReturn();         // +Z
float4 EyeUp    = _EyeUp.NormalizeReturn();          // +Y
float4 EyeRight = Cross3D(EyeUp, EyeDir).NormalizeReturn(); // +X

ArrVector[0] = EyeRight;
ArrVector[1] = EyeUp;
ArrVector[2] = EyeDir;

float4 NegEye = -_EyePos;
float D0 = Dot3D(EyeRight, NegEye);
float D1 = Dot3D(EyeUp,    NegEye);
float D2 = Dot3D(EyeDir,   NegEye);

Transpose();                      // row → column
ArrVector[3] = { D0, D1, D2, 1.0f };
```

내적이 들어가는 이유가 헷갈릴 수 있는데, "각 축으로 카메라를 얼마나 밀어야 원점이 되는지를 각 축에 투영해 계산"하는 것이다.

결과(열 기준):
```
| R.x  U.x  F.x  Tx |
| R.y  U.y  F.y  Ty |
| R.z  U.z  F.z  Tz |
|  0    0    0    1 |
```

프로젝트에서는 카메라의 Transform이 가진 Forward/Up/Position을 그대로 넘긴다:

```cpp
float4 EyePos = GetTransform()->GetWorldPosition();
float4 EyeDir = GetTransform()->GetLocalForwardVector();
float4 EyeUp  = GetTransform()->GetLocalUpVector();
ViewMatrix.LookToLH(EyePos, EyeDir, EyeUp);
```

View Space 축 규약:
| 방향 | 의미 |
|---|---|
| +X | 오른쪽 |
| +Y | 위 |
| +Z | 앞 |

## 프로젝션 행렬 (Projection Matrix)

View Space까지는 여전히 3D. 모니터는 2D이므로 **투영(projection)** 이 필요하다. 원근(Perspective)과 직교(Orthographic) 두 종류가 있고, 3D 게임은 거의 원근.

### `XMMatrixPerspectiveFovLH(FovY, Aspect, NearZ, FarZ)`

| 파라미터 | 의미 |
|---|---|
| `FovY` | 수직 시야각(라디안). 가로 시야각은 Aspect로 보정 |
| `Aspect` | `Width / Height` |
| `NearZ` | 이보다 가까우면 컬 |
| `FarZ` | 이보다 멀면 컬 |

내부 로직을 풀어 쓰면:

```cpp
float YScale = 1.0f / tanf(FovY / 2.0f);
float XScale = YScale / Aspect;
float ZRange = FarZ - NearZ;

Arr2D[0][0] = XScale;
Arr2D[1][1] = YScale;
Arr2D[2][2] = FarZ / ZRange;
Arr2D[2][3] = 1.0f;                          // w ← z (W-Divide 준비)
Arr2D[3][2] = -NearZ * FarZ / ZRange;
Arr2D[3][3] = 0.0f;
```

### 각 항목이 하는 일

**`YScale = 1 / tan(FovY / 2)`** — 화면 세로 방향 스케일.
`tan(FovY/2) = y / z` 관계에서 y 값을 [-1, 1]에 맞추는 역수.

**`XScale = YScale / Aspect`** — 가로는 세로를 종횡비로 보정.
세로 FOV는 고정하고 가로는 창 크기에 맞춰 자동으로 따라간다.

**Z 매핑** — Near→0, Far→1로 맞추되 선형이 아니다.
`z_ndc = A + (B/z)` 꼴로 **비선형 매핑**을 해서 멀리 있는 물체가 원근감 있게 작아진다.
Near·Far 넣고 연립방정식을 풀면:
- `A = Zf / (Zf - Zn)`
- `B = -Zn * Zf / (Zf - Zn)`

**`[3][3] = 0`** — Clip 공간의 w를 z로만 채우기 위한 상수항 제거.

### 예시로 감 잡기

FOV 60°, 16:9, Near 0.1, Far 1000이면:
```
yScale ≈ 1.732
xScale ≈ 0.974
zCompress ≈ 1.0001
zBias ≈ -0.10001

| 1.732   0       0        0 |
|   0     0.974   0        0 |
|   0     0       1.0001   1 |
|   0     0      -0.10001  0 |
```

View Space 점 `(2, 3, 10, 1)` 에 이걸 곱하면 Clip `(1.948, 5.196, 9.901, 10)`.
Rasterizer가 W-Divide를 돌려 NDC `(0.1948, 0.5196, 0.9901, 1)`.

주목할 점 — X/Y가 같아도 Z가 크면 크기가 작아진다:
- `(2, 2, 2, 2)` → NDC `(1.0, 1.0, 1.0)`
- `(2, 2, 10, 10)` → NDC `(0.2, 0.2, 0.2)`
- `(2, 2, 50, 50)` → NDC `(0.04, 0.04, 0.04)`

이게 원근(Perspective)이다.

## 뷰포트 행렬 (Viewport)

D3D11은 뷰포트 변환을 **하드웨어 고정 기능**으로 처리한다. 그래서 함수로 행렬을 만들진 않고 `D3D11_VIEWPORT` 구조체를 `RSSetViewports()`로 바인딩하면 끝.

내부 수식:
```
ScreenX = (NDC.x + 1.0) * 0.5 * VP.Width  + VP.TopLeftX
ScreenY = (1.0 - NDC.y) * 0.5 * VP.Height + VP.TopLeftY
ScreenZ = NDC.z * (MaxDepth - MinDepth) + MinDepth
```

## 프레임워크에서 한 번에 묶기

`Ext_Camera::CameraTransformUpdate()`에서 매 프레임 View/Projection/Viewport를 갱신:

```cpp
void Ext_Camera::CameraTransformUpdate()
{
    float4 EyePos = GetTransform()->GetWorldPosition();
    float4 EyeDir = GetTransform()->GetLocalForwardVector();
    float4 EyeUp  = GetTransform()->GetLocalUpVector();
    ViewMatrix.LookToLH(EyePos, EyeDir, EyeUp);

    if (CameraType == ProjectionType::Perspective)
        ProjectionMatrix.PerspectiveFovLH(FOV, Width / Height, Near, Far);

    ViewPortMatrix.ViewPort(Width, Height, 0.0f, 0.0f);
}
```

각 메시의 Transform이 WVP 조합을 들고 있다:

```cpp
void TransformData::SetViewProjectionMatrix(
    const float4x4& _View, const float4x4& _Projection)
{
    ViewMatrix               = _View;
    ProjectionMatrix         = _Projection;
    WorldViewMatrix          = WorldMatrix * ViewMatrix;
    WorldViewProjectionMatrix= WorldMatrix * ViewMatrix * ProjectionMatrix;
}
```

WVP를 VS에서 한 번에 곱해도 되지만, 중간 행렬을 **디버그용으로 보존**한다.

## 얻은 결과

- **정사각형이 제대로 정사각형으로 보인다**. 종횡비 문제 해결.
- 원근 효과가 생긴다. Z가 먼 물체는 작게, 가까우면 크게.
- 카메라 Transform 하나 움직이면 전체 장면이 자연스럽게 따라오는 **공간 기준점**이 확보된다.
- 이후 모든 씬·액터·라이트가 이 카메라를 기준으로 그려질 수 있게 됨.

## 다음 단계 / 개선거리

- [카메라 개선 세션 WorkLog](../WorkLog/2026-04-19.md) — 최근 추가된 **언리얼 에디터 스타일 flycam**, **SpringArm 3인칭** 은 이 카메라 시스템 위에서 구현됨
- **직교 투영** — `OrthographicLH` 계산 붙이면 2D UI/미니맵용 직교 카메라 가능
- **Reversed-Z** — 멀리까지 정밀도 좋은 depth 유지를 위해 반전 Z 사용 고려 (HDR 파이프라인 같이 하면 효과적)
- **다중 카메라** — 현재도 지원되지만 다중 RT 경로에 테스트 필요

## 관련 현재 코드
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) `CameraTransformUpdate`
- [DirectX11_Extension/Ext_Transform.h](../../DirectX11_Extension/Ext_Transform.h) `TransformData::SetViewProjectionMatrix`
- [DirectX11_Base/Base_Math.h](../../DirectX11_Base/Base_Math.h) `LookToLH`, `PerspectiveFovLH`, `ViewPort`
