---
title: DirectXTex를 통한 Texture 활용
original_url: https://umtimos.tistory.com/194
original_date: 2025-05-28
migrated: 2026-04-19
category: DirectX
---

# DirectXTex를 통한 Texture 활용

색만 달린 정점으론 아무리 예쁘게 그려봐야 한계가 있다. 진짜 질감이 들어가려면 **텍스처**가 필요한데, DirectX11이 내장한 텍스처 로더가 빈약해서(특히 PNG/JPG 같은 일반 포맷) Microsoft가 제공하는 **DirectXTex** 라이브러리를 끌어와 쓴다.

## DirectXTex 라이브러리 준비

[Microsoft/DirectXTex GitHub](https://github.com/microsoft/DirectXTex)에서 소스를 받아 Static Lib으로 빌드한다. 본 프로젝트는 빌드된 `.lib`을 `ThirdParty/DirectXTex/lib.zip`으로 동봉해놨다 — `Unzip.bat` 한 번 돌리면 배치된다.

구조:
```
ThirdParty/DirectXTex/
├── inc/
│   ├── DirectXTex.h
│   └── DirectXTex.inl
└── lib/x64/
    ├── Debug/DirectXTex.lib
    └── Release/DirectXTex.lib
```

프로젝트 설정:
- VC++ **Include Directories** : `..\ThirdParty\DirectXTex\inc\`
- **Library Directories** : `$(SolutionDir)ThirdParty\DirectXTex\lib\x64\$(Configuration)`
- **Additional Dependencies** : `DirectXTex.lib`

현재 프로젝트는 이 설정이 vcxproj에 이미 들어있다 — 새 머신에서 빌드할 땐 `Unzip.bat`만 기억하면 된다.

## 텍스처 로드

DirectXTex는 PNG/JPG/BMP 같은 일반 포맷을 WIC로, 게임 업계 표준인 DDS를 전용 로더로, 그리고 TGA까지 지원한다. 확장자만 보고 맞는 로더를 분기:

```cpp
void Ext_DirectXTexture::TextureLoad(
    std::string_view _Path, std::string_view _ExtensionName)
{
    std::wstring Path = Base_String::AnsiToUniCode(_Path);
    std::string  Ext  = Base_String::ToUpper(_ExtensionName.data());

    if      (Ext == ".TGA")
        DirectX::LoadFromTGAFile(Path.c_str(), DirectX::TGA_FLAGS_NONE, &TexData, Image);
    else if (Ext == ".DDS")
        DirectX::LoadFromDDSFile(Path.c_str(), DirectX::DDS_FLAGS_NONE, &TexData, Image);
    else
        DirectX::LoadFromWICFile(Path.c_str(), DirectX::WIC_FLAGS_NONE, &TexData, Image);

    DirectX::CreateShaderResourceView(
        Ext_DirectXDevice::GetDevice(),
        Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(),
        SRV.GetAddressOf());
}
```

반환되는 것:
- `TexMetadata` — 폭·높이·포맷·밉 레벨 등 정보
- `ScratchImage` — 실제 픽셀 데이터
- `CreateShaderResourceView` — D3D SRV를 바로 만들어 준다

## 픽셀 셰이더에서 사용

```hlsl
Texture2D    DefaultTex : register(t0);
SamplerState Sampler    : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR;
    float2 TexCoord : TEXCOORD;
};

float4 Basic_PS(PSInput In) : SV_TARGET
{
    return DefaultTex.Sample(Sampler, In.TexCoord);
}
```

`Texture2D.Sample(Sampler, UV)` 이 한 줄이 핵심. 프레임워크에선 `PSSetShaderResources`/`PSSetSamplers` 가 `Ext_DirectXBufferSetter`를 통해 자동 바인딩된다.

## 런타임 텍스처 교체

같은 메시/머티리얼이라도 다른 텍스처를 쓰고 싶을 때가 많다. `Ext_MeshComponent::SetTexture(textureName)`로 슬롯에 텍스처 이름을 연결하면 된다:

```cpp
MeshComp->SetTexture("StoneWall_BaseColor.jpg", TextureType::BaseColor);
MeshComp->SetTexture("StoneWall_Normal.jpg",   TextureType::Normal);
```

내부적으로는 `BufferSetter`가 이름→SRV 매핑을 기억하고, 렌더링 시 바인딩한다.

## 얻은 결과

- **PNG, JPG, TGA, DDS 전부 로드 가능**. 게임 아트 파이프라인에서 쓰는 거의 모든 포맷이 열림.
- 텍스처는 **리소스 매니저 캐시**에 이름으로 올라가므로, 같은 파일을 두 번 로드해도 한 번만 실제 I/O 발생.
- 디퍼드 파이프라인에서 BaseColor / Normal / Roughness 등 여러 슬롯을 동시에 물리는 기반이 여기서 깔렸다.

## 주의할 점 / 개선거리

- **밉맵이 자동 생성되지 않는다** — DDS 포맷이 아닌 경우 직접 `DirectX::GenerateMipMaps()`를 호출해야 함. 현재는 로드만 하고 GenerateMipMaps 미사용
- **sRGB 처리** — `R8G8B8A8_UNORM` vs `R8G8B8A8_UNORM_SRGB` 구분이 일관되지 않음. BaseColor는 sRGB, Normal은 Linear 원칙으로 명시화 필요
- **텍스처 압축** — BC5(Normal)·BC7(Color) 같은 GPU 압축 포맷으로 전환하면 VRAM 사용량 크게 줄어듦. 현재는 원본 PNG/JPG 그대로라 메모리 많이 먹음
- **비동기 로드** — 현재 동기 로드라 씬 전환 시 프리즈. 백그라운드 스레드로 옮길 여지

## 다음 단계

- [Texcoord(UV)와 Sampler](10-UV-Sampler.md) — 로드된 텍스처를 어떻게 샘플링할지
- [DirectXTK CubeMap 로드 (SkyBox)](../Framework/13-SkyBox.md) — DirectXTex가 아닌 DirectXTK로 큐브맵 처리

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXTexture.{h,cpp}](../../DirectX11_Extension/Ext_DirectXTexture.h)
- [DirectX11_Extension/Ext_DirectXBufferSetter.cpp](../../DirectX11_Extension/Ext_DirectXBufferSetter.cpp) `TextureSetting()`
- [DirectX11_Extension/Ext_MeshComponent.cpp](../../DirectX11_Extension/Ext_MeshComponent.cpp) `SetTexture()`
