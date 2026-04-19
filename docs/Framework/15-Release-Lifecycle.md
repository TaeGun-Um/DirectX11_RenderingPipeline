---
title: 오브젝트 수명 주기 (Destroy / Release)
original_url: ""
original_date: 2026-04-20
migrated: 2026-04-20
category: Framework
---

# 오브젝트 수명 주기 — Destroy / Release

게임 프레임워크에서 오브젝트(Actor, Component)를 생성하는 것보다 **지우는 것이 훨씬 까다롭다**. 아무 때나 지웠다간 Update 루프 중에 iterator 가 날아가고, 너무 느리게 지웠다간 죽은 상태로 한 프레임 더 그려지거나 느려진다. 이 문서는 그 균형을 맞춘 지금 프레임워크의 수명 주기 구조를 설명한다.

## 두 단계: Destroy(예약) → Release(실제 정리)

개념을 분리한 이유 — **순회 중 iterator 무효화를 피하기 위해**. 모든 제거는 지연된다.

| 단계 | 의미 | 시점 |
|---|---|---|
| `Destroy()` | "나 죽을게요" 플래그 + 대기열 등록 | 언제든 호출 가능 (Update 중 포함) |
| `Release()` | 실제 자원 해제 + 컨테이너 erase | 프레임 끝, 안전한 시점 |

## 세 레이어 구조

프레임워크 오브젝트는 **Scene → Actor → Component** 의 3층 계층이다. 각 층이 자기 밑의 Dead 대기열을 관리한다:

```
Ext_Scene
    │   bHasDeadActor (flag)
    │   PendingDeadActors (vector)
    │
    ├── Ext_Actor
    │       │   bHasDeadComponent (flag)
    │       │   PendingDeadComponents (vector)
    │       │
    │       └── Ext_Component
    │               OnDetachFromScene()  (virtual)
    │
    └── ...
```

## 흐름 — Actor 하나 지우기

예: `BoxActor->Destroy()` 를 호출했을 때

```
 [Update 루프 중]
   BoxActor->Destroy()
   ├── bIsDeath = true
   └── OwnerScene.lock()->MarkDeadActor(self)
          ├── Scene.PendingDeadActors.push_back(self)
          └── Scene.bHasDeadActor = true
   (Update 루프 계속 진행 — Actor 는 아직 Components 맵 안에 살아 있음)

 [프레임 끝, Ext_Core::Update 의 끝부분]
   CurrentScene->Release()
   ├── if (!bHasDeadActor) return;          ← fast path 가드
   ├── for DeadActor in PendingDeadActors:
   │     ├── for Comp in DeadActor.Components:
   │     │     Comp->OnDetachFromScene()    ← MeshComp 는 Camera 에서 자기 제거
   │     └── DeadActor->Release()           ← 내부 Component 자원 정리
   ├── Actors 맵에서 IsDeath() 인 항목 erase
   └── PendingDeadActors.clear(), bHasDeadActor = false
```

## 흐름 — Component 하나 지우기

예: `MeshComp->Destroy()` 호출 시

```
 [Update 루프 중]
   MeshComp->Destroy()
   ├── bIsDeath = true
   └── OwnerActor.lock()->MarkComponentDead(self)
          ├── Actor.PendingDeadComponents.push_back(self)
          └── Actor.bHasDeadComponent = true

 [다음 프레임의 Actor::Update 초입]
   if (bHasDeadComponent) {
       RemoveDeadComponents()
       ├── for DeadComp in PendingDeadComponents:
       │     ├── DeadComp->OnDetachFromScene()    ← MeshComp → Camera 제거
       │     ├── DeadComp->Release()              ← Transform 등 자원 정리
       │     └── Components.erase(Name)
       ├── PendingDeadComponents.clear()
       └── bHasDeadComponent = false
   }
```

## 핵심 설계 포인트

### 1. Fast-Path 가드

가장 큰 최적화. 평상시(죽은 게 없는 프레임) 에는 **`if (!bHasDeadActor) return;`** 한 줄로 빠져나간다.

```cpp
void Ext_Scene::Release()
{
    if (!bHasDeadActor) return; // ← 99%의 프레임은 여기서 종료
    // ... 실제 정리 로직
}
```

### 2. 지연 삭제 + 대기열

Destroy 에서 즉시 지우면:
```cpp
for (auto& [n, comp] : Components)     // 순회 중
{
    if (comp->ShouldDie())
        Components.erase(...);          // ← iterator 무효화!
}
```
이런 걸 피하기 위해 **대기열에 push** 만 해두고, 안전한 시점에 한 번에 처리.

### 3. `OnDetachFromScene()` 가상함수

RTTI 없이 "컴포넌트 본인이 자기 정리 책임" 을 지도록 한 패턴:

```cpp
class Ext_Component
{
    virtual void OnDetachFromScene() {} // default no-op
};

class Ext_MeshComponent : public Ext_Component
{
    void OnDetachFromScene() override
    {
        // 내가 등록돼 있는 카메라의 MeshComponent 목록에서 나를 제거
        if (auto Cam = OwnerCamera.lock())
            Cam->RemoveMeshByActor(GetOwnerActor().lock());
    }
};
```

**과거**에는 Scene::Release 가 `dynamic_pointer_cast<Ext_MeshComponent>()` 로 타입을 확인해가며 직접 정리 코드를 썼다. 지금은 다형성으로 자동 분기 — 응집도 ↑, RTTI 비용 ↓.

### 4. 재귀 Destroy 옵션

Actor::Destroy(true) 나 Component::Destroy(true) 로 자식까지 같이 죽일 수 있다:
- Actor 의 경우 보유 Component 들
- Component 의 경우 Transform 계층의 자식 Component 들

기본값 false 로 의도치 않은 연쇄 삭제를 방지.

## 순환 참조 / 안전성 이슈

### `shared_from_this` 유효성

`Destroy()` 가 `GetSharedFromThis<T>()` 를 호출하는데, 이 객체가 어딘가의 `shared_ptr` 에 담겨 있어야 유효. 프레임워크 내에서는 `Scene::CreateActor` / `Actor::CreateComponent` 경로로만 생성하므로 항상 만족.

### 중복 Destroy 방지

모든 `Destroy()` 는 `if (bIsDeath) return;` 가드로 시작. 여러 번 호출돼도 대기열에 중복 등록 안 됨.

### Release 도중의 참조

Release 순서:
1. Component 의 `OnDetachFromScene` — Scene-level 등록 해제
2. Component 의 `Release` — Transform 등 자원 해제
3. Actor 의 `Components.erase`

이 순서로 해야 "Component 가 카메라에 등록된 채로 erase 되어 dangling pointer 생성" 같은 경우를 방지.

## 얻은 결과

- **죽은 게 없는 프레임** 의 비용이 O(N)에서 O(1)로 감소.
- **죽은 게 있는 프레임** 에도 전체 순회 대신 대기열만 순회. 또한 RTTI 캐스트 제거.
- 구조가 명확해져 **디버깅·확장** 이 쉬움. 새 컴포넌트 타입은 `OnDetachFromScene` override 만 추가하면 자동 정리됨.

## 시각화 테스트 (ReleaseTestActor)

구조가 제대로 도는지 **눈으로 확인** 할 수 있는 도우미 액터를 마련:

- 매 1초 주기로 Cube/Sphere 10개를 원형 배치 → Destroy → 다음 주기에 다시 10개
- `RenderScene` 에 1개만 배치하면 자동 반복
- 체크리스트:
  - 깜빡임·끊김 없이 전환되는가
  - 메모리가 일정하게 유지되는가 (VS Diagnostic)
  - F6 셰이더 Hot Reload 와 병행해도 간섭 없는가

관련 파일: [`DirectX11_Contents/ReleaseTestActor.{h,cpp}`](../../DirectX11_Contents/ReleaseTestActor.h) — 학습/디버깅용. 프로덕션 빌드에서는 `RenderScene` 의 한 줄 주석 처리로 끔.

### 이 테스트로 발견된 버그
**순회 중 CreateActor → vector 재할당 → freed 메모리 접근** 패턴이 드러남. `Ext_Scene::ActorsUpdate` 가 `Actors[Order]` 벡터를 range-for 로 돌리는 중, ReleaseTestActor 가 같은 벡터에 `push_back` 하며 참조 무효화 → `0xDDDD...` crash. 해결: **스냅샷 복사 후 순회**. 상세: [2026-04-20 WorkLog §7](../WorkLog/2026-04-20.md).

Destroy 쪽은 이미 "지연 삭제" 로 이 문제를 막았지만 **Create 쪽은 놓치고 있었음**. 시각화 테스트의 효용을 잘 보여준 사례.

## 다음 단계 / 개선거리

- **`std::list<shared_ptr>` 전환** — 현재 `std::vector<shared_ptr>` 의 erase 가 죽은 프레임에 O(N). list 로 바꾸면 O(1). Order 그룹당 수십 개 정도면 큰 차이 없지만 수천 개면 체감.
- **Delayed destroy queue 병합** — 현재 Actor/Scene/Update 각 단계에서 개별 처리. Ext_Core 레벨의 통합 큐로 관리하면 단계별 복잡도 감소 가능.
- **컴포넌트 타입별 매니저** — Collision / Reflection 컴포넌트도 각 매니저에서 Remove 해야 하는데, 지금은 Collision 이 Scene.Collisions 에 등록돼 있음. `OnDetachFromScene` override 로 자기 제거 구현 필요.

## 관련 현재 코드
- [DirectX11_Extension/Ext_Scene.{h,cpp}](../../DirectX11_Extension/Ext_Scene.h) — `MarkDeadActor`, `Release`
- [DirectX11_Extension/Ext_Actor.{h,cpp}](../../DirectX11_Extension/Ext_Actor.h) — `Destroy`, `MarkComponentDead`, `RemoveDeadComponents`
- [DirectX11_Extension/Ext_Component.{h,cpp}](../../DirectX11_Extension/Ext_Component.h) — `Destroy`, `OnDetachFromScene`
- [DirectX11_Extension/Ext_MeshComponent.{h,cpp}](../../DirectX11_Extension/Ext_MeshComponent.h) — `OnDetachFromScene` override
- [2026-04-20 WorkLog](../WorkLog/2026-04-20.md) — 이번 변경의 구체 이력
