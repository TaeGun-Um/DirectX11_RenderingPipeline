---
title: 프레임워크 기능 추가 - 3 + 간단한 Lighting 해보기 (FSM·Collision·Directional Light)
original_url: https://umtimos.tistory.com/198
original_date: 2025-06-04
migrated: 2026-04-19
category: Framework
---

# 프레임워크 기능 추가 - 3 + 간단한 Lighting

애니메이션이 붙은 캐릭터를 움직여보려면 "**지금 걷기 중인지, 공격 중인지**" 같은 상태 전이가 필요하다. 또 지형과의 충돌이 없으면 공중에 떠다니거나 벽을 관통하니 **Collision** 도 빠질 수 없다. 그리고 명암이 없으면 3D가 납작해 보이므로 간단한 **Directional Light**까지 이번 단계에서 추가한다.

## 1. FSM (Finite State Machine)

캐릭터는 Idle / Walk / Attack 같은 제한된 **상태**를 가지고, 입력이나 이벤트에 따라 **한 상태에서 다른 상태로 전이**한다. 이걸 깔끔하게 만드는 가장 단순한 방법이 FSM.

### 설계

각 상태는 세 가지 라이프사이클 훅을 가진다:

| 훅 | 호출 시점 |
|---|---|
| `Start` | 그 상태로 들어올 때 한 번 |
| `Update(dt)` | 그 상태에 머무는 동안 매 프레임 |
| `End` | 다른 상태로 나갈 때 한 번 |

사용 예:
```cpp
PlayerFSM.CreateState({
    .StateValue = Character_FSM::Idle,
    .Start  = [=] { BodyMesh->SetAnimation("Idle", true); },
    .Update = [=](float dt)
    {
        if (Base_Input::IsPress("Forword"))
        {
            PlayerFSM.ChangeState(Character_FSM::Walking);
            return;
        }
        // ...
    },
});
```

한 상태 안에서 로직이 국소화돼 있어서 "걷는 중일 때만 속도 누적" 같은 게 깔끔해진다.

### 왜 FSM으로 충분한가

행동 트리(Behavior Tree)처럼 더 복잡한 모델도 있지만, 본 프로젝트는 **단일 플레이어블 캐릭터 + 소수 상태** 이므로 FSM이 가장 가성비 좋다. AI NPC가 붙거나 상태가 수십 개가 되면 BT로 확장 고려.

## 2. Collision

충돌은 형태별로 세 가지를 지원:

```cpp
enum class CollsionType
{
    Sphere3D,
    AABB3D,
    OBB3D,
    Unknown,
};
```

DirectXCollision의 `BoundingSphere`, `BoundingBox(AABB)`, `BoundingOrientedBox(OBB)`를 내부에서 쓴다. 모든 3D 쉐이프가 비슷한 `Intersects()` 시그니처를 가지므로 switch 하나로 대응 가능.

### Union 트릭

각 타입이 필요한 필드가 다르지만 사이즈는 비슷하다. Union으로 겹쳐 쓰면 메모리 낭비 없이 타입 스위칭이 가능:

```cpp
union
{
    DirectX::BoundingSphere       Sphere;
    DirectX::BoundingBox          AABB;
    DirectX::BoundingOrientedBox  OBB;
};
```

`BoundingOrientedBox`의 `Center` / `Extents` / `Orientation`만 업데이트하면 위 세 개가 동시에 일관된 값을 갖는 효과 (메모리 오버랩 활용).

### CollisionGroup

단순 도형 충돌 이상으로, "플레이어만 적과 충돌" 같은 **그룹 필터**가 필요:

```cpp
enum class CollisionGroup
{
    Player,
    PlayerAttack,
    Platform,
    Wall,
    Box,
    Unknown
};
```

각 컴포넌트는 자기 그룹을 들고 있고, `Collision(targetGroup)`로 특정 그룹과만 검사.

## 3. 간단한 Lighting — Directional

가장 단순한 광원. 방향만 있고 위치는 없음 (태양광이라고 보면 됨).

### Light 데이터 구조

```cpp
struct LightData
{
    float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.1f };  // RGB=색, w=Ambient 강도
    float4 LightWorldPosition;
    float4 LightForward;
    float  LightNear = 1.0f;
    float  LightFar  = 1.0f;
    int    LightType = 0;    // 0 = Directional, 1 = Point, ...
};
```

씬은 `Ext_Scene`이 Lights 리스트를 들고, 매 프레임 Update에서 상수버퍼로 보냄.

### 픽셀 셰이더 (Lambert Diffuse)

가장 기본인 람베르트 확산 반사:

```hlsl
float3 Normal    = normalize(_Input.Normal.xyz);
float3 LightDir  = normalize(-LightForward.xyz);  // 표면→광원 방향
float  Diff      = max(dot(Normal, LightDir), 0.0f);

float3 LitColor  = Albedo.rgb * Diff * LightColor.rgb;
return float4(LitColor, Albedo.a);
```

`dot(Normal, LightDir)` 이 1에 가까울수록 정면, 0이면 수평, 음수면 반대편(그늘).

VS에서 노멀을 월드 공간으로 변환해 넘길 때 주의:
```hlsl
Output.WorldNormal = mul(float4(Input.Normal.xyz, 0.0f), WorldMatrix).rgb;
```
w=0 인 이유는 위치가 아닌 방향 벡터라 평행이동이 적용되면 안 되기 때문.

## 얻은 결과

- 캐릭터가 **상태 기반으로 움직이고 애니메이션이 전환**됨 (Idle ↔ Walk ↔ Attack 등).
- 지형 Collision 덕에 캐릭터가 **바닥 위에 서 있고, 벽에 안 뚫림**.
- Directional Light 한 줄기로도 3D 느낌이 확 사는 걸 확인. 평면 셰이딩 대비 입체감 크게 향상.
- 이후 본격적인 **PBR / Shadow / Deferred** 로 확장하기 위한 공통 LightData 구조가 잡힘.

## 다음 단계 / 개선거리

- [Point Light + Phong Specular 추가](07-Lighting.md) — 점광과 하이라이트
- **Shadow Mapping** — 광원 시점에서 Depth를 한 번 더 그려 그림자 생성
- **Ambient를 라이트에 포함한 현재 구조** — 사실 Ambient는 전역 1개로 두는 게 더 자연스러움. 리팩토링 후보
- **Collision broad phase** — 현재는 전체 O(N²) 검사. 많아지면 공간 분할(Quadtree/BVH) 필요
- **FSM → Behavior Tree** — 상태 수가 늘면 Flat FSM은 스파게티가 된다

## 관련 현재 코드
- [DirectX11_Extension/Ext_FSM.{h,cpp}](../../DirectX11_Extension/Ext_FSM.h)
- [DirectX11_Extension/Ext_CollisionComponent.{h,cpp}](../../DirectX11_Extension/Ext_CollisionComponent.h)
- [DirectX11_Extension/Ext_Light.{h,cpp}](../../DirectX11_Extension/Ext_Light.h)
- [DirectX11_Contents/Character.cpp](../../DirectX11_Contents/Character.cpp) — FSM 실제 사용 사례
- [Shader/LightData.fx](../../Shader/LightData.fx)
