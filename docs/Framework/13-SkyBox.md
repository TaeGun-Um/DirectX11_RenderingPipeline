---
title: 스카이박스 만들어보기
original_url: https://umtimos.tistory.com/212
original_date: 2025-06-21
migrated: 2026-04-19
category: Framework
---

# 스카이박스 만들어보기

야외 씬을 그리면 카메라 뒤·위·옆에 늘 **배경**이 필요하다. 완전한 하늘·구름·먼 산을 지오메트리로 그리면 비싸니까, 카메라를 감싸는 **거대한 큐브 내부에 텍스처를 붙인** 방식으로 대체한다. 이게 스카이박스.

## 3단계 구성

1. **큐브맵 텍스처 준비** — 6면 이미지로 큐브맵 생성
2. **스카이박스 머티리얼 구성** — 전용 Depth state · 위치 고정 로직
3. **출력 확인**

## 큐브맵 텍스처

큐브맵은 6장의 이미지가 `[+X, -X, +Y, -Y, +Z, -Z]` 순서로 들어간 특수 텍스처. DirectX11은 `D3D11_RESOURCE_MISC_TEXTURECUBE` 플래그로 만든다.

본 프로젝트는 `Resource/FX/ReflectionTexture/SkyBox/` 에 6장의 PNG를 두고 일괄 로드:

```cpp
Base_Directory Dir;
Dir.MakePath("../Resource/FX/ReflectionTexture/SkyBox");
std::vector<std::string> Paths = Dir.GetAllFile({ "png" });

std::vector<std::shared_ptr<Ext_DirectXTexture>> Texs;
for (const std::string& FilePath : Paths)
    Texs.push_back(Ext_DirectXTexture::LoadTexture(FilePath.c_str()));

Ext_DirectXTexture::LoadCubeMap("SkyBox", Texs);
```

`LoadCubeMap()` 내부:
- `D3D11_TEXTURE2D_DESC`에 `ArraySize = 6`, `MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE`
- `D3D11_SUBRESOURCE_DATA[6]` 에 각 면의 원본 픽셀 데이터를 채워 `CreateTexture2D`
- SRV는 `D3D_SRV_DIMENSION_TEXTURECUBE` 로 생성

## 머티리얼 — Depth WriteMask=ZERO

스카이박스의 핵심 규칙: **"항상 가장 먼 곳"** 처럼 취급돼야 한다. 그래서 Depth 테스트는 하되 **기록은 안 한다**. 이미 가까이 그려진 오브젝트가 있으면 그 앞에 덮어쓰지 않도록:

```cpp
D3D11_DEPTH_STENCIL_DESC Desc = { 0 };
Desc.DepthEnable    = true;
Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;   // ★ 쓰기 OFF
Desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
Desc.StencilEnable  = false;

Ext_DirectXDepth::CreateDepthStencilState("SkyDepth", Desc);
```

이후 `SkyBox` 머티리얼은 `SetDepthState("SkyDepth")`.

## 셰이더

### VS — 카메라 따라다니게 + z=w 트릭

큐브가 카메라 위치를 따라다녀야 하늘이 움직이지 않는 것처럼 보인다. 또 깊이 값을 **극한인 1(w=1로 맞춤)** 로 두면 다른 모든 오브젝트가 앞에 그려질 수 있다:

```hlsl
cbuffer TransformData : register(b0) { float4x4 WorldViewProjectionMatrix; };

struct VSInput  { float4 Position : POSITION; };
struct VSOutput { float4 Position : SV_POSITION; float3 TexCoord : TEXCOORD0; };

VSOutput SkyBox_VS(VSInput In)
{
    VSOutput Out;
    Out.Position    = mul(In.Position, WorldViewProjectionMatrix);
    Out.Position.z  = Out.Position.w;     // z/w = 1 → 가장 먼 depth
    Out.TexCoord    = In.Position.xyz;    // 큐브맵 방향 = 로컬 위치
    return Out;
}
```

Transform은 `WorldPosition = Camera.WorldPosition` 으로 매 프레임 업데이트 — 그래야 카메라가 이동해도 하늘이 따라온다.

### PS — 큐브맵 샘플

```hlsl
TextureCube CubeMapTex : register(t0);
SamplerState Sampler   : register(s0);

float4 SkyBox_PS(PSInput In) : SV_TARGET
{
    return CubeMapTex.Sample(Sampler, In.TexCoord);
}
```

`TextureCube.Sample(sampler, direction)` — 3D 방향 벡터로 샘플. VS에서 넘긴 로컬 위치가 방향 벡터 역할.

## 별도 RT로 그리고 합성

`SkyBoxRenderTarget` 에 따로 그린 뒤, 최종 합성 단계에서 `CameraRT->Merge(SkyBoxRT)` 하는 구조:

```cpp
// Ext_Camera::SkyBoxRendering()
SkyBoxRenderTarget->RenderTargetClear();
SkyBoxRenderTarget->RenderTargetSetting();

float4 CamPos = GetTransform()->GetWorldPosition();
SkyBoxTransform->SetLocalPosition(CamPos);
SkyBoxTransform->SetCameraMatrix(CamPos, ViewMatrix, ProjectionMatrix);

SkyBoxUnit.GetBufferSetter().SetConstantBufferLink(
    "TransformData", *SkyBoxTransform->GetTransformData());
SkyBoxUnit.Rendering(0.0f);
```

왜 별도 RT인가 — 디퍼드의 MeshRT는 6장 MRT라서 포맷이 안 맞고, 나중에 LightMerge 전후에 자연스럽게 섞기 위함.

## 얻은 결과

- 카메라를 돌려도 **하늘이 항상 화면을 꽉 채움**. 씬에 공간감이 확 생김.
- 스카이박스 자체는 **Depth 기록 안 하므로** 다른 오브젝트와의 전후관계 자연스럽게 유지.
- 이후 Reflection Probe가 이 SkyBox 큐브맵을 초기 환경 맵으로도 쓸 수 있게 됨.

## 주의할 점 / 개선거리

- **HDR SkyBox** — PNG 8bit 큐브맵은 밝은 태양 표현이 부족. HDR DDS / EXR 이미지 쓰면 반사·블룸 품질 크게 향상
- **프로시저럴 스카이** — 텍스처 대신 태양 위치·시간·날씨 기반으로 그라디언트를 계산(Preetham, Hosek-Wilkie). 리얼타임 시간 변화에 강함
- **IBL (Image-Based Lighting)** — 스카이박스를 디퓨즈/스페큘러 환경 맵으로 프리컨볼루션해 PBR 조명에 사용. 현재 프레임워크는 Reflection Probe 단계에서 부분적으로 활용
- **큐브맵 해상도/포맷** — 현재 각 면 크기가 원본 PNG 그대로. 밉맵 생성과 포맷 통일 필요
- **파일 로드 순서 보장** — `GetAllFile` 이 파일 이름 알파벳 순인지 OS/FS에 따라 다를 수 있음. `[+X, -X, +Y, -Y, +Z, -Z]` 순서 명시 주의

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXTexture.cpp](../../DirectX11_Extension/Ext_DirectXTexture.cpp) — `LoadCubeMap`
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) — `SkyBoxRendering`
- [Shader/SkyBox_VS.hlsl](../../Shader/SkyBox_VS.hlsl)
- [Shader/SkyBox_PS.hlsl](../../Shader/SkyBox_PS.hlsl)
- [DirectX11_Contents/SkyBoxActor.cpp](../../DirectX11_Contents/SkyBoxActor.cpp)
