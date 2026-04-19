---
title: SwapChain과 RenderTarget 생성
original_url: https://umtimos.tistory.com/150
original_date: 2025-05-16
migrated: 2026-04-19
category: DirectX
---

# SwapChain과 RenderTarget 생성

Device만 만들어두고 그냥 그림을 그리면 아무것도 안 보인다. 그려진 픽셀이 **어디로 나가야 화면에 보이는지** 아직 알려주지 않았기 때문이다. 그 연결을 담당하는 게 **SwapChain**과 **RenderTarget**이다.

## 왜 SwapChain인가

GPU는 프레임을 아주 빠르게 찍어낸다. 한 장의 버퍼에 계속 덮어쓰면:

1. 그리는 도중에 화면에 나가면 **절반만 그려진 프레임**이 보일 수 있다 — 티어링
2. 이전 내용을 Clear 안 하면 **잔상**이 남는다

그래서 버퍼 두 장을 준비해서 한쪽(Back Buffer)에 그리고 완성되면 화면(Front)과 교체한다. 이게 **더블 버퍼링**이고, 이 역할을 DirectX에서는 `IDXGISwapChain`이 맡는다.

## SwapChain 생성 4단계

코드 순서는 거의 정해져 있다:

1. Device에서 `IDXGIDevice` 가져오기 (`QueryInterface`)
2. `IDXGIDevice`에서 `IDXGIAdapter` 가져오기 (`GetParent`)
3. `IDXGIAdapter`에서 `IDXGIFactory` 가져오기 (`GetParent`)
4. Factory의 `CreateSwapChain`으로 스왑체인 만들기

```cpp
void Ext_DirectXDevice::CreateSwapChain()
{
    float4 ScreenSize = Base_Windows::GetScreenSize();
    DXGI_SWAP_CHAIN_DESC Desc = { 0 };

    Desc.BufferCount                       = 2;                        // 더블 버퍼링
    Desc.BufferDesc.Width                  = ScreenSize.uix();
    Desc.BufferDesc.Height                 = ScreenSize.uiy();
    Desc.OutputWindow                      = Base_Windows::GetHWnd();
    Desc.BufferDesc.RefreshRate.Numerator   = 60;                      // 60Hz
    Desc.BufferDesc.RefreshRate.Denominator = 1;
    Desc.BufferDesc.Format                 = DXGI_FORMAT_R8G8B8A8_UNORM;
    Desc.BufferDesc.ScanlineOrdering       = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    Desc.BufferDesc.Scaling                = DXGI_MODE_SCALING_UNSPECIFIED;
    Desc.BufferUsage                       = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    Desc.SampleDesc.Count                  = 1;                        // MSAA off
    Desc.SampleDesc.Quality                = 0;
    Desc.SwapEffect                        = DXGI_SWAP_EFFECT_FLIP_DISCARD;  // 최신 D3D11 권장
    Desc.Flags                             = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    Desc.Windowed                          = true;

    COMPTR<IDXGIDevice>  SwapDevice;
    COMPTR<IDXGIAdapter> SwapAdapter;
    COMPTR<IDXGIFactory> SwapFactory;

    Device->QueryInterface(__uuidof(IDXGIDevice), (void**)SwapDevice.GetAddressOf());
    SwapDevice ->GetParent(__uuidof(IDXGIAdapter), (void**)SwapAdapter.GetAddressOf());
    SwapAdapter->GetParent(__uuidof(IDXGIFactory), (void**)SwapFactory.GetAddressOf());

    if (S_OK != SwapFactory->CreateSwapChain(Device.Get(), &Desc, SwapChain.GetAddressOf()))
    {
        MsgAssert("SwapChain 생성 실패");
        return;
    }

    // 백버퍼 텍스처 획득 → RTV 생성 → 메인 렌더타겟 등록
    COMPTR<ID3D11Texture2D> BackTex;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)BackTex.GetAddressOf());

    auto Tex = std::make_shared<Ext_DirectXTexture>();
    Tex->CreateRenderTargetView(BackTex);

    BackBufferTarget = Ext_DirectXRenderTarget::CreateRenderTarget(
        "MainRenderTarget", Tex, { 0.f, 0.f, 1.f, 1.f });
    BackBufferTarget->CreateDepthTexture();
}
```

## Desc 항목 요약

| 필드 | 설명 |
|---|---|
| `BufferCount` | 2 — 더블 버퍼링 |
| `BufferDesc.Width/Height` | 백버퍼 해상도 |
| `OutputWindow` | 결과를 내보낼 창의 HWND |
| `RefreshRate` | 60/1 → 60Hz |
| `BufferDesc.Format` | `R8G8B8A8_UNORM` — 채널당 8bit |
| `BufferUsage` | RTV로도 쓰고 SRV로도 읽을 수 있게 |
| `SampleDesc` | MSAA 설정 (off면 Count=1, Quality=0) |
| `SwapEffect` | `FLIP_DISCARD` — 최신 표준, 성능·VRR 호환 |
| `Windowed` | 창 모드 여부 |

`FLIP_DISCARD`는 예전 `DISCARD`보다 빠르고 DWM과 잘 맞는다. 특별한 이유 없으면 이게 기본값이 돼야 한다.

## DXGI가 뭔가

D3D 아래쪽에서 하드웨어·OS와 신호를 주고받는 계층이다. 일반적으로는 D3D를 통해 간접적으로 쓰지만, **감마 컬렉션**이나 **디스플레이 모드 변경** 같은 작업은 DXGI를 직접 만져야 한다.

## RenderTarget이란

한 문장으로 말하면 "렌더링 결과가 찍힐 GPU 메모리 공간". 보통 이 네 가지가 한 세트로 묶인다:

- **`ID3D11Texture2D`** — 2D 이미지 원본
- **`ID3D11RenderTargetView` (RTV)** — 그 텍스처에 색을 기록하기 위한 출구
- **`ID3D11ShaderResourceView` (SRV)** — 다른 셰이더가 이걸 읽기 위한 입구
- **`D3D11_VIEWPORT`** — NDC → 픽셀 매핑

RTV 생성 본체는 다음과 같다:

```cpp
void Ext_DirectXTexture::CreateRenderTargetView(COMPTR<ID3D11Texture2D>& _Tex)
{
    Texture2D = _Tex;
    Texture2D->GetDesc(&Texture2DInfo);

    COMPTR<ID3D11RenderTargetView> NewRTV;
    if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRenderTargetView(
        Texture2D.Get(), nullptr, NewRTV.GetAddressOf()))
    {
        MsgAssert("RTV 생성 실패");
        return;
    }
    RTVs.push_back(NewRTV);
}
```

## Depth / Stencil Buffer

3D 씬을 그리려면 **누가 앞이고 누가 뒤인지**를 알아야 한다. 그걸 픽셀 단위로 저장하는 버퍼가 Depth 버퍼(Z-Buffer). Stencil은 같이 묶여서 특정 영역만 그리거나 안 그리는 마스크 용도로 쓴다.

```cpp
void Ext_DirectXRenderTarget::CreateDepthTexture(int _Index)
{
    D3D11_TEXTURE2D_DESC Desc = { 0 };
    Desc.ArraySize = 1;
    Desc.Width  = Textures[_Index]->GetWidth();
    Desc.Height = Textures[_Index]->GetHeight();
    Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24bit depth + 8bit stencil
    Desc.SampleDesc.Count   = 1;
    Desc.SampleDesc.Quality = 0;
    Desc.MipLevels = 1;
    Desc.Usage     = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    DepthTexture = Ext_DirectXTexture::CreateViews(Desc);
}
```

`DXGI_FORMAT_D24_UNORM_S8_UINT`는 "앞 24bit는 0~1 범위 깊이, 뒤 8bit는 0~255 스텐실". 대부분의 경우 이 포맷으로 충분하다.

## 백 버퍼에 뭔가 찍어보기

모든 셋팅이 끝나면 루프 안에서 이렇게 쓴다:

```cpp
auto MainRT  = Ext_DirectXDevice::GetMainRenderTarget();
auto RTV     = MainRT->GetTexture(0)->GetRTV();
auto DSV     = MainRT->GetDepthTexture()->GetDSV();
auto VP      = MainRT->GetViewPort(0);

float ClearColor[4] = { 0.f, 0.f, 1.f, 1.f };    // 짙은 파랑
auto* ctx = Ext_DirectXDevice::GetContext();

ctx->ClearRenderTargetView(RTV.Get(), ClearColor);
ctx->ClearDepthStencilView(DSV.Get(),
    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

ctx->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
ctx->RSSetViewports(1, VP);

Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
```

`Present(SyncInterval, Flags)`의 `SyncInterval=1`은 V-Sync를 건다는 뜻이다. 0을 주면 tearing을 감수하고 최대 프레임레이트로 쏴줄 수도 있다.

## 얻은 결과

- **화면이 파란색으로 칠해지는** 결과. 소소하지만 "내 GPU에 내가 원하는 걸 찍고 Present까지 태웠다"는 걸 확인할 수 있는 첫 신호다.
- 백버퍼와 Depth 버퍼가 한 쌍으로 묶여 **렌더타겟 추상화**의 기초가 깔렸다. 이후에 나올 디퍼드 렌더링이나 포스트 프로세스에서 이 구조를 그대로 재사용한다.
- RTV / SRV를 같이 만들어둬서, 한 번 그린 텍스처를 **다른 패스에서 읽을 수 있는** 길이 열렸다 (Merge / 포스트 프로세스에 필수).

## 다음 단계

- [InputAssembler 단계](04-InputAssembler.md) — 이 화면에 진짜로 삼각형을 그려보는 다음 단계
- 개선거리:
  - **Resize 처리** — 현재는 창 크기 변경 시 SwapChain을 다시 만들지 않는다. WM_SIZE에서 `ResizeBuffers` 호출을 붙이면 더 안정적
  - **HDR 스왑체인** — `DXGI_FORMAT_R10G10B10A2_UNORM` / `R16G16B16A16_FLOAT`로 확장 가능. 스카이박스/반사가 많아질수록 체감이 크다
  - **가변 프레임레이트 (VRR)** — `DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING`을 켜고 Present 플래그 조정

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXDevice.cpp](../../DirectX11_Extension/Ext_DirectXDevice.cpp) `CreateSwapChain()`
- [DirectX11_Extension/Ext_DirectXRenderTarget.h](../../DirectX11_Extension/Ext_DirectXRenderTarget.h)
- [DirectX11_Extension/Ext_DirectXTexture.cpp](../../DirectX11_Extension/Ext_DirectXTexture.cpp)
