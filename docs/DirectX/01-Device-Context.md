---
title: Device와 Context 생성하기
original_url: https://umtimos.tistory.com/139
original_date: 2025-05-14
migrated: 2026-04-19
category: DirectX
---

# Device와 Context 생성하기

DirectX11로 뭔가를 그리려면 가장 먼저 해야 할 일이 **Device와 Context를 여는 것**이다. Device는 "GPU에 자원을 할당하는 창구"이고, Context는 "그 자원을 가지고 실제 드로우 콜을 날리는 실행자"다. 둘이 쌍으로 움직인다고 생각하면 편하다.

## 최소 요구사항

| 항목 | 내용 |
|---|---|
| OS | Windows 10 이상 (개발 환경 Windows 11) |
| IDE | Visual Studio 2022 (Platform Toolset `v143` 이상) |
| SDK | Windows 10 SDK 이상 (DirectX 11 헤더·라이브러리 내장) |
| 언어 | ISO C++20 |
| 플랫폼 | x64 |
| 외부 라이브러리 | Assimp, DirectXTex, DirectXTK, Dear ImGui |

## 헤더와 라이브러리

VS2022는 Windows SDK에 DirectX 11이 이미 들어있어 별도 설치가 필요 없다. PCH나 공통 헤더에서 한 번 포함시키면 프로젝트 전반에서 쓸 수 있다.

```cpp
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <D3D11Shader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
```

| 헤더 | 역할 |
|---|---|
| `d3d11.h` | Device / Context / 각종 View 등 D3D11 코어 API |
| `dxgi.h` | SwapChain / Factory / Adapter — 그래픽 표시 인프라 |
| `d3dcompiler.h` | HLSL 런타임 컴파일 |
| `D3D11Shader.h` | 컴파일된 셰이더 리플렉션 |

라이브러리는 `.lib`만 링크하면 된다. `dxguid.lib`는 COM 인터페이스들의 GUID 상수가 들어있어서 `__uuidof()` 사용 시 필요.

## Device와 Context 만들기

실제 코드는 아래처럼 간결하다. 눈여겨볼 건 **디버그 플래그**와 **Adapter**를 직접 골라 전달하는 부분이다.

```cpp
void Ext_DirectXDevice::Initialize()
{
    if (!Base_Windows::GetHWnd())
    {
        MsgAssert("윈도우 초기화 전에는 Initialize를 진행할 수 없습니다.");
        return;
    }

    int iFlag = 0;
#ifdef _DEBUG
    iFlag = D3D11_CREATE_DEVICE_DEBUG;   // 디버그 레이어 → 문제 생기면 OutputDebug에 상세 로그
#endif

    D3D_FEATURE_LEVEL Level = D3D_FEATURE_LEVEL_11_0;
    COMPTR<IDXGIAdapter> Adapter = GetHighPerformanceAdapter();
    if (!Adapter) { MsgAssert("그래픽카드 어댑터 획득 실패"); return; }

    HRESULT Hr = D3D11CreateDevice(
        Adapter.Get(),
        D3D_DRIVER_TYPE_UNKNOWN,     // Adapter 직접 넘기므로 UNKNOWN
        nullptr,
        iFlag,
        nullptr, 0,
        D3D11_SDK_VERSION,
        Device.GetAddressOf(),
        &Level,
        Context.GetAddressOf());

    if (S_OK != Hr) { MsgAssert("디바이스 생성 실패"); return; }
    if (Level != D3D_FEATURE_LEVEL_11_0)
    {
        MsgAssert("DirectX 11을 지원하지 않는 그래픽카드입니다");
        return;
    }

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    CreateSwapChain();
}
```

> 블로그 원문은 `IDXGIAdapter*` 원시 포인터를 썼지만 지금은 전부 `COMPTR<IDXGIAdapter>`로 바뀌었다. 이 래핑 구조체 이야기는 [03-ComPtr](03-ComPtr.md)에서.

## 고성능 어댑터 직접 선택

외장·내장이 둘 다 있는 노트북에선 아무 어댑터나 쓰면 내장 GPU에 붙을 수 있다. 그래서 **전용 VRAM이 가장 큰 어댑터**를 고르는 로직을 직접 넣었다:

```cpp
COMPTR<IDXGIAdapter> Ext_DirectXDevice::GetHighPerformanceAdapter()
{
    COMPTR<IDXGIFactory>  Factory;
    COMPTR<IDXGIAdapter>  Best;

    CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)Factory.GetAddressOf());
    if (!Factory) { MsgAssert("Factory 생성 실패"); return {}; }

    size_t MaxVram = 0;
    for (UINT i = 0; ; ++i)
    {
        COMPTR<IDXGIAdapter> Cur;
        if (Factory->EnumAdapters(i, Cur.GetAddressOf()) == DXGI_ERROR_NOT_FOUND) break;

        DXGI_ADAPTER_DESC Desc;
        Cur->GetDesc(&Desc);
        if (Desc.DedicatedVideoMemory >= MaxVram)
        {
            MaxVram = Desc.DedicatedVideoMemory;
            Best = Cur;
        }
    }
    return Best;
}
```

## `D3D11CreateDevice` 파라미터

| 파라미터 | 의미 |
|---|---|
| `IDXGIAdapter*` | 쓸 어댑터. nullptr이면 첫 번째 |
| `D3D_DRIVER_TYPE` | Adapter 직접 지정 시 `UNKNOWN` |
| `HMODULE` | 소프트웨어 래스터라이저 DLL 핸들 (보통 nullptr) |
| `UINT` 플래그 | `D3D11_CREATE_DEVICE_DEBUG` 등 |
| `const D3D_FEATURE_LEVEL*` | 시도할 기능 수준 배열 |
| `UINT` | 위 배열 길이 |
| `UINT` | `D3D11_SDK_VERSION` |
| `ID3D11Device**` | 생성된 Device 받을 주소 |
| `D3D_FEATURE_LEVEL*` | 실제 생성된 기능 수준 |
| `ID3D11DeviceContext**` | 생성된 Context 받을 주소 |

## 얻은 결과

이 단계만 끝나도 다음 것들이 가능해진다:

- **GPU와 통신할 수 있는 입구**가 열렸다. Device로 버퍼·텍스처·셰이더를 만들고, Context로 실제로 그릴 수 있다.
- Debug 빌드에서 **D3D11 디버그 레이어**가 출력창에 무슨 상태가 꼬였는지 알려주게 된다. 리소스 누수, 바인딩 누락 같은 게 실시간으로 잡힌다.
- 멀티 GPU 환경에서도 **고성능 GPU를 강제로 선택**하게 돼서 외장 GPU의 성능을 그대로 쓸 수 있다.

## 다음 단계

- [SwapChain / RenderTarget](02-SwapChain-RenderTarget.md) — Device로 만든 게 실제 화면에 나오려면 SwapChain이 필요
- [ComPtr](03-ComPtr.md) — COM 객체 수명을 안전하게 관리하기 위한 래퍼
- 추후 개선거리:
  - Feature Level 배열을 전달해 `11_1` 이상도 시도하도록 확장
  - `D3D11CreateDeviceAndSwapChain`로 묶지 않고 별도로 만드는 현재 구조는 DXGI 핫-스왑이 쉬워서 유지

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXDevice.h](../../DirectX11_Extension/Ext_DirectXDevice.h)
- [DirectX11_Extension/Ext_DirectXDevice.cpp](../../DirectX11_Extension/Ext_DirectXDevice.cpp)
