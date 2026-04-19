---
title: 리플렉션 프로브 해보기
original_url: https://umtimos.tistory.com/213
original_date: 2025-06-21
migrated: 2026-04-19
category: Framework
---

# 리플렉션 프로브

금속 구체나 유리 표면은 **주변 환경을 비춰 보여야** 진짜처럼 보인다. 실시간 레이트레이싱 없이 이걸 흉내 내는 방법이 **리플렉션 프로브** — 씬의 특정 지점에서 6방향을 찍어 큐브맵으로 만들고, 그 큐브맵을 표면 반사로 샘플링하는 기법.

## 원리

1. 원하는 위치(프로브 포인트)에 임시 카메라를 둔다
2. 카메라를 6방향으로 돌려가며 렌더 (±X, ±Y, ±Z)
3. 6장의 이미지를 큐브맵으로 합성
4. 반사 오브젝트의 PS에서 **반사 방향** 으로 큐브맵 샘플

실시간 레이트레이싱 대비:
- 프레임마다 새로 찍으면 비싸니 **정적 프로브** 로 한 번만 찍고 재사용
- 위치 한 곳 기준이라 프로브에서 멀어질수록 부정확(다수 프로브로 보완)

## Ext_ReflectionComponent

리플렉션을 담당하는 컴포넌트. Actor 위치를 프로브 포인트로 삼는다.

### 초기화 — `ReflectionInitialize()`

```cpp
void ReflectionInitialize(
    std::shared_ptr<Ext_Actor> _Owner,
    std::string_view _CaptureTextureName,
    const float4& _Scale = float4(128, 128))
{
    // 1. 이미 저장된 큐브맵 있으면 재사용
    std::string ForwardFile = _CaptureTextureName + "_Forword.png";
    if (!Ext_DirectXTexture::Find(ForwardFile))
    {
        // 2. 없으면 6방향 캡처
        float4 CenterPos = _Owner->GetTransform()->GetWorldPosition();

        // 메인 카메라를 프로브 위치로 임시 이동시켜가며 6회 렌더
        auto Camera = _Owner->GetOwnerScene().lock()->GetMainCamera();
        Camera->CaptureCubemap(CenterPos, /* rot */, /* cap size */);
        CaptureTarget->Merge(Camera->GetCameraRenderTarget());
        Ext_ScreenShoot::RenderTargetShoot_DxTex(
            CaptureTarget, Path, _CaptureTextureName + "_Forword.png");

        // Back, Right, Left, Top, Bottom 반복
        // ...
    }

    // 3. 6장 PNG를 큐브맵으로 로드
    std::vector<std::shared_ptr<Ext_DirectXTexture>> CubeTextures;
    CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName + "_Right.png"));
    CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName + "_Left.png"));
    CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName + "_Top.png"));
    CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName + "_Bottom.png"));
    CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName + "_Forword.png"));
    CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName + "_Back.png"));

    CubeTexture = Ext_DirectXTexture::LoadCubeMap(_CaptureTextureName, CubeTextures);
}
```

### Screenshot — `Ext_ScreenShoot::RenderTargetShoot_DxTex`

RT 내용을 PNG로 떨어뜨리는 건 DirectXTK의 `SaveWICTextureToFile`를 쓴다. 이렇게 파일로 저장하면 **다음 실행 때 재캡처 없이 로드** 가능 — 정적 프로브의 핵심 최적화.

### `AttachTo` 헬퍼 (2026-04 리팩토링)

블로그 원문에선 ReflectionActor, StoneWallActor에 중복 코드가 있었는데 (심지어 static 카운터가 이름 충돌하는 버그), 리팩토링하며 정적 헬퍼로 통합:

```cpp
static std::shared_ptr<Ext_ReflectionComponent> AttachTo(
    std::shared_ptr<Ext_Actor> _Owner,
    std::shared_ptr<Ext_MeshComponent> _Mesh,
    const float4& _CaptureSize = float4(512, 512));
```

사용처에서는:
```cpp
Reflection = Ext_ReflectionComponent::AttachTo(
    GetSharedFromThis<Ext_Actor>(), MeshComp);
```

한 줄이면 큐브 캡처 + 큐브맵 텍스처 생성 + 메시에 바인딩 완료. 이름 충돌 버그도 제거 ([WorkLog](../WorkLog/2026-04-19.md)).

## 반사 계산 — Pixel Shader

표면 법선과 뷰 방향으로 반사 벡터를 구하고 큐브맵 샘플:

```hlsl
TextureCube  ReflectionTexture : register(t1);
SamplerState CubeMapSampler    : register(s2);

float3 ViewDir    = normalize(CamPos - In.WorldPosition);
float3 Normal     = normalize(In.WorldNormal);
float3 ReflectDir = normalize(2.0f * Normal * dot(ViewDir, Normal) - ViewDir);

float4 ReflectionColor = ReflectionTexture.Sample(CubeMapSampler, ReflectDir);
```

실제로는 Albedo / Diffuse 와 적당히 섞어서 최종 컬러 결정.

## Actor에서 사용

```cpp
void ReflectionActor::SetReflection()
{
    Reflection = Ext_ReflectionComponent::AttachTo(
        GetSharedFromThis<Ext_Actor>(),
        MeshComp);
}

// 메인 Scene에서
void RenderScene::Start()
{
    auto Reflect = CreateActor<ReflectionActor>("ReflectionActor1");
    Reflect->GetTransform()->SetLocalPosition({ 0, 100, -200 });
    Reflect->SetReflection();  // 한 번만 호출 → 큐브맵 자동 생성/로드
}
```

## 얻은 결과

- 반사 오브젝트가 **주변 씬을 비춰 보여줌**. 단순한 PBR 효과보다 직관적이고 재밌음.
- 최초 1회만 PNG 캡처, 이후 실행엔 재사용 — 비용이 실질적으로 0.
- Mirror-like 표면부터 매트한 반사까지 셰이더에서 조절 가능.

## 주의할 점 / 개선거리

- **동적 씬에 약함** — 씬이 바뀌거나 NPC가 움직이면 프로브는 옛날 상태를 보여준다. 간격을 두고 재캡처하는 로직 필요
- **Parallax Correction** — 프로브 위치 기준이라 프로브에서 멀어지면 반사 왜곡. 박스 크기 정보를 주고 보정(Local Cubemap) 가능
- **블러 레벨** — 매끈한 면은 선명, 러프한 면은 흐릿한 반사가 자연스러움. 큐브맵을 prefilter해 밉맵별 블러 수준 달리 저장하는 PBR convolve 필요
- **HDR** — PNG 8bit 포맷이라 밝은 부분이 날아감. HDR DDS로 저장하면 반사 품질 향상
- **정적 캡처 결정적이지 않음** — 첫 캡처 시점의 씬이 결과에 그대로 박힘. 자산 변경 시 수동 재캡처 필요 (파일 삭제 후 재실행)

## 히스토리

- **2025-06-21**: 블로그 원글 — 구조/기능 소개
- **2026-04-19**: [AttachTo 헬퍼로 통합](../WorkLog/2026-04-19.md#3-중복-제거-리팩토링-phase-2) — ReflectionActor/StoneWallActor 중복 + 이름 충돌 해결

## 관련 현재 코드
- [DirectX11_Extension/Ext_ReflectionComponent.{h,cpp}](../../DirectX11_Extension/Ext_ReflectionComponent.h)
- [DirectX11_Extension/Ext_ScreenShoot.{h,cpp}](../../DirectX11_Extension/Ext_ScreenShoot.h) — `RenderTargetShoot_DxTex`
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) — `CaptureCubemap`
- [DirectX11_Contents/ReflectionActor.cpp](../../DirectX11_Contents/ReflectionActor.cpp)
- [DirectX11_Contents/StoneWallActor.cpp](../../DirectX11_Contents/StoneWallActor.cpp)
- [Shader/GraphicsCUBE_PS.hlsl](../../Shader/GraphicsCUBE_PS.hlsl) — 큐브맵 반사 PS
