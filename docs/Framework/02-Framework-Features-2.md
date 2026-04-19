---
title: 프레임워크 기능 추가 - 2 (프로젝트 구성·Flow·Scene/Actor/Component)
original_url: https://umtimos.tistory.com/181
original_date: 2025-05-25
migrated: 2026-04-19
category: Framework
---

# 프레임워크 기능 추가 - 2

첫 Feature 추가에서 유틸리티 베이스를 깔았다면, 이번엔 **"하나의 프레임을 어떻게 계속 돌릴 건지"** 의 뼈대를 세우는 단계다. 매 프레임 수동으로 Clear/SetRenderTarget/Draw를 쓰는 건 실험용은 되지만 지속 가능하지 않다. 씬과 오브젝트가 알아서 갱신·렌더링되는 **루프 아키텍처**가 필요하다.

## 왜 이 단계가 필요한가

앞 포스트까지는:
- 도형 띄우기 → 실행하고 Alt+F4로 끄기
- 매 프레임 수동 세팅
- 렌더 결과 1회성

이후로는 게임·그래픽스 학습 용도로 **씬 여러 개를 돌려보고, 오브젝트를 붙였다 뗐다** 하고 싶다. 그래서 다음이 필요:
- 메인 루프 (`Window Loop`)
- 씬 개념 (`Scene`)
- 오브젝트·컴포넌트 모델 (`Actor` / `Component`)
- 프레임 독립성 (`Deltatime`)
- 입력 추상화 (`Input`)

## 프로젝트 구성

솔루션 하나에 **4개 lib + 1개 exe + Shader** 구조. 레이어별로 역할이 다르다:

| 프로젝트 | 종류 | 역할 |
|---|---|---|
| `DirectX11_Base` | Static Lib | 유틸 (윈도우, `float4`, 델타타임, 입력 등) |
| `DirectX11_Extension` | Static Lib | 엔진 Loop + DirectX 모듈 전체 |
| `DirectX11_Contents` | Static Lib | 사용자 씬·액터 코드 |
| `DirectX11_RenderingPipeline` | Exe | `WinMain`, 실행 진입점 |
| `Shader` | (HLSL 빌드) | 런타임에 로드될 HLSL 파일들 |

의존 방향은 Base → Extension → Contents → exe.

왜 lib으로 쪼갰나:
- Base·Extension은 **재사용 가능한 엔진** 부분
- Contents는 **게임(또는 데모) 특화** 부분
- 나중에 새 프로젝트에서 엔진만 가져다 쓸 수 있게 독립성 유지

## 프레임워크 Flow

대략적인 흐름은 이렇다:

```
Window Create
       │
Level(Scene) Create
       │
┌──────▼──────────────────────────┐
│  Program Loop                   │
│                                 │
│   Level Change Check            │
│   Deltatime 계산                │
│   Level.Update(dt)              │
│     └── Actor·Component Update  │
│   Rendering Setting             │
│     └── RTV Clear / Set         │
│     └── Camera Matrix Update    │
│     └── Mesh Rendering          │
│   Present (백버퍼 → 화면)       │
│   Release (죽은 오브젝트 정리)  │
└──────┬──────────────────────────┘
       │ 창이 살아있는 동안 반복
       ▼
Window End
```

실제 코드는 `Ext_Core::Run` → `Base_Windows::WindowLoop` → `Ext_Core::Update`의 체인이다.

## 추가된 기능

### 1. Scene

액터·컴포넌트·카메라·라이트를 묶은 컨테이너. 여러 개 만들어 두고 런타임에 전환 가능(`Ext_Core::ChangeScene`). 씬 전환 시 `SceneChangeStart` / `SceneChangeEnd` 훅이 호출돼 리소스 정리/준비를 맡길 수 있다.

### 2. Actor

Transform을 기본 보유한 월드의 오브젝트. 컴포넌트를 탈부착해 기능 확장. 액터를 직접 상속해 특화(예: `CubeActor`, `Character`)하는 것도 가능.

### 3. Component

액터에 붙는 기능 단위. Mesh(`Ext_MeshComponent`), Collision, Reflection, SpringArm 등. 자체 Transform이 있으며, 붙을 때 액터의 Transform을 부모로 삼는다. 그래서 액터가 움직이면 컴포넌트도 같이 따라 움직인다.

### 4. Transform

Local/World 두 세트의 행렬을 갖는다:
- 부모 없음 → Local == World
- 부모 있음 → Local × Parent.World = World

이렇게 분리해 두면 캐릭터의 손에 붙은 무기, 카메라에 부착된 SpringArm 같은 **계층 구조**가 자연스럽게 동작한다.

### 5. Input (초기)

카메라 조작·간단한 키 입력용. `GetAsyncKeyState()`를 매 프레임 폴링해 `IsDown` / `IsPress` / `IsFree`로 질의.

> 초기엔 단순 폴링이었지만 최근 리팩토링으로 **Axis·마우스 델타·휠·커서 잠금·리바인드**까지 지원하도록 대폭 확장됐다. 자세한 건 [2026-04-19 WorkLog](../WorkLog/2026-04-19.md) 참조.

## 얻은 결과

- 메인 루프 한 번에 `CubeActor->Update(dt)` / `PushMeshToCamera` / `Render` 가 자동으로 돌아가게 됨.
- 씬 바꾸고 싶으면 `Ext_Core::ChangeScene("Other")` 한 줄.
- 액터/컴포넌트 구조 덕분에 "구면을 하나 더 추가"가 클래스 상속 한 번 + 씬에서 `CreateActor` 한 번으로 끝.

## 다음 단계 / 개선거리

- **Compute/Geometry 파이프라인** — 현재는 VS/PS 한정
- **Scene 전환 중 리소스 해제 기준** — 지금은 전역 ResourceManager가 앱 종료 시까지 쥐고 있음. 씬 전환 시 특정 텍스처만 해제 같은 기능 없음
- **Component 의존 순서** — SpringArm이 Transform을 읽는 Update 순서를 보장해야 함. 현재는 삽입 순서 기준으로 암묵적 의존이지만, 명시적 우선순위 API 필요할 수 있음
- **Actor 지연 소멸** — `Destroy()` 호출 시 다음 `Release` 단계에서 실제 삭제. 두 번 호출되거나 참조 걸린 상태 주의 ([코드 리뷰 메모](../WorkLog/2026-04-19.md#1-코드-리뷰-크리티컬-버그-수정))

## 관련 현재 코드
- [DirectX11_Extension/Ext_Core.{h,cpp}](../../DirectX11_Extension/Ext_Core.h) — 엔진 루프
- [DirectX11_Extension/Ext_Scene.{h,cpp}](../../DirectX11_Extension/Ext_Scene.h)
- [DirectX11_Extension/Ext_Actor.{h,cpp}](../../DirectX11_Extension/Ext_Actor.h)
- [DirectX11_Extension/Ext_Component.{h,cpp}](../../DirectX11_Extension/Ext_Component.h)
- [DirectX11_Extension/Ext_Transform.{h,cpp}](../../DirectX11_Extension/Ext_Transform.h)
- [DirectX11_Base/Base_Input.{h,cpp}](../../DirectX11_Base/Base_Input.h)
- [DirectX11_Base/Base_Windows.{h,cpp}](../../DirectX11_Base/Base_Windows.h)
