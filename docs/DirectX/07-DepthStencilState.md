---
title: DepthStencilState 생성
original_url: https://umtimos.tistory.com/190
original_date: 2025-05-28
migrated: 2026-04-19
category: DirectX
---

# DepthStencilState 생성

3D 씬에서 가장 귀찮고 또 가장 중요한 문제가 "**어떤 픽셀이 앞에 있나**"다. 그걸 자동으로 처리해주는 게 Depth 버퍼고, 그 버퍼를 **어떻게 해석할지**를 정하는 규칙이 `DepthStencilState`다.

## View와 State는 다른 것

DirectX에서 비슷해 보이는 이름 둘이 있다:
- **`ID3D11DepthStencilView` (DSV)** — "어떤 메모리를 Depth/Stencil 저장소로 쓸지" 가리키는 뷰
- **`ID3D11DepthStencilState`** — "그 저장소를 어떻게 해석·기록할지" 정의하는 상태

View는 **버퍼**, State는 **설정**. 렌더링엔 둘 다 필요하다.

## Z-Buffer와 Z-Culling

Depth 버퍼는 픽셀마다 0~1 범위의 깊이 값을 저장한다. 새 픽셀이 들어왔을 때 "내가 이미 저장된 것보다 가까우면 그리고, 아니면 버린다"는 비교(**Z-Test**)를 하는데, 이 비교 함수가 State에서 정해진다(`LESS`, `LESS_EQUAL`, `ALWAYS` 등).

덕분에 뒤에 있는 물체를 뒤에서 그리든 앞에서 그리든 **올바른 가려짐**이 자동으로 된다. 단, 반투명은 이야기가 다르다 — Z-Buffer만으로 결정되지 않으므로 [back-to-front 정렬](../Framework/12-Transparency-Test.md)이 별도로 필요하다.

## Stencil Buffer

깊이 24bit 뒤에 붙는 8bit 스텐실. 거울·포털·아웃라인 같은 **마스킹**에 쓴다. 본 프레임워크는 기본적으로 꺼둔 상태지만, `MergeDepth`·`AlphaDepth` 같은 최신 state에서 명시적으로 비활성화하도록 정리해 뒀다.

## 기본 DepthStencilState

일반 불투명 오브젝트용 — 테스트도 하고 기록도 한다:

```cpp
void Ext_DirectXResourceLoader::MakeDepth()
{
    D3D11_DEPTH_STENCIL_DESC Desc = { 0 };
    Desc.DepthEnable    = true;                         // depth 테스트 ON
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // depth 기록 ON
    Desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;  // 가깝거나 같으면 통과
    Desc.StencilEnable  = false;

    Ext_DirectXDepth::CreateDepthStencilState("EngineDepth", Desc);
}
```

필드별 의미:

| 필드 | 의미 |
|---|---|
| `DepthEnable` | Depth Test 사용 |
| `DepthWriteMask` | `ZERO`면 쓰기 OFF, `ALL`이면 쓰기 ON |
| `DepthFunc` | 비교 함수 (`LESS`, `LESS_EQUAL`, `ALWAYS` 등) |
| `StencilEnable` | 스텐실 사용 |

`LESS_EQUAL`을 쓴 이유는 **같은 depth 값을 통과시키기 위함**. 디퍼드 라이팅에서 라이트 패스가 G-Buffer depth와 같은 depth로 들어와야 하는 경우에 쓸모있다.

## 프로젝트에 등록된 State 목록

[Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) `MakeDepth()`에 현재 다음이 있다:

| 이름 | DepthEnable | Write | Func | Stencil | 쓰임 |
|---|---|---|---|---|---|
| `EngineDepth` | ON | ALL | LESS_EQUAL | OFF | 일반 불투명 |
| `AlwayDepth` | ON | ALL | ALWAYS | OFF | 풀스크린 쿼드(옛 Merge) |
| `SkyDepth` | ON | ZERO | LESS_EQUAL | OFF | 스카이박스 — 깊이 기록 안 함 |
| `StencilDepth` / `StencilTest` | ... | ... | ... | ON | 스텐실 테스트 |
| `AlphaDepth` (신규) | ON | ZERO | LESS_EQUAL | OFF | 반투명 Forward — 읽기만 |
| `MergeDepth` (신규) | OFF | — | — | OFF | RT 간 Merge — depth 안 건드림 |

### 최근 수정 포인트

`AlphaDepth`와 `MergeDepth`는 [2026-04-19 알파 패스 재구성](../WorkLog/2026-04-19.md#5-디퍼드-알파-패스-정리) 중에 추가됐다. 기존 Merge 머티리얼이 `AlwayDepth`(WriteMask=ALL)를 쓰는 바람에 풀스크린 쿼드가 opaque depth를 덮어썼고, 이어지는 알파 Forward 패스가 깨지는 버그가 있었다. Merge 계열은 `MergeDepth`(완전 OFF)로, 반투명 Forward는 `AlphaDepth`(쓰기 OFF)로 정리했다.

## OutputMerger 단계에서의 역할

파이프라인 최종 단계(OM)는 다음 순서:

1. **Depth Test** — 깊이 비교 통과한 픽셀만
2. **Stencil Test** — 스텐실 마스크 확인
3. **Blending** — BlendState에 따라 색 합성
4. RT에 기록

State를 묶어 적용하는 함수:
```cpp
ctx->OMSetDepthStencilState(State.Get(), stencilRef);
```

## 얻은 결과

- **3D 가려짐이 자동**으로 된다. 앞/뒤 정렬 신경 쓰지 않고 그려도 제대로 된 결과가 나옴.
- **상황별 state 분리**가 가능해졌다. 스카이박스는 기록 안 하고, 반투명은 읽기만, Merge는 둘 다 off 같은 식으로.
- 이후 디퍼드 렌더링, 포스트 프로세스, 반투명 Forward 패스 전부 이 state 위에서 돌아감.

## 다음 단계 / 개선거리

- **Reversed-Z** — `DepthFunc = GREATER_EQUAL` + Clear 값 0.0f 로 뒤집으면 멀리까지 precision 이 크게 좋아진다. HDR 파이프라인과 같이 쓸만함
- **Early-Z 활용** — 불투명 메시들을 **near→far 정렬**로 그리면 GPU가 뒤에 가려지는 픽셀을 VS도 실행 안 함. 현재는 far→near 정렬인데 반투명과 공유하느라 손해가 있음
- **Stencil 활용 케이스** — 포털/거울/아웃라인 같은 기능 붙일 때 StencilDepth/StencilTest state 재활용

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXDepth.{h,cpp}](../../DirectX11_Extension/Ext_DirectXDepth.h)
- [DirectX11_Extension/Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp) `MakeDepth()`
