---
title: Microsoft::WRL::ComPtr<T>
original_url: https://umtimos.tistory.com/172
original_date: 2025-05-19
migrated: 2026-04-19
category: DirectX
---

# Microsoft::WRL::ComPtr<T>

DirectX를 만지다 보면 금방 깨닫는 게 있다. **Release 호출을 까먹기 시작한다**는 것. D3D11의 거의 모든 인터페이스가 COM 객체이고, COM은 `AddRef()` / `Release()`로 참조 카운트를 직접 관리한다. 실수로 한두 개 빼먹으면 앱 종료 시점에 "Live Object 몇 개 남음"이라는 경고가 쏟아진다.

그래서 이 프로젝트는 전부 **`Microsoft::WRL::ComPtr<T>`** 로 감싸 쓴다. 이걸 다시 얇게 래핑한 `COMPTR<T>` 를 프로젝트 공용 타입으로 두고 있다.

## COM과 `shared_ptr`이 맞물리지 않는 이유

D3D 인터페이스들은 전부 `IUnknown`을 상속받는 COM 객체다. 이들은 내부에 자체 RAII + Reference Counting을 갖고 있어서 `std::shared_ptr`로 감쌀 수 없다(소멸자 접근 제한). 그래서 원시 포인터로 받으면 개발자가 직접 `Release()` 해야 한다.

누수가 생기면 `_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF)` 같은 CRT 릭 체크에 의해 종료 시 아래처럼 경고가 찍힌다:

> D3D11 WARNING: Live Device... Live Object at 0x... Refcount: 1

## `ComPtr<T>`의 동작

본질적으로는 스마트 포인터다. 생성·대입·복사될 때 `AddRef`, 소멸될 때 `Release`를 자동 호출한다.

### 1. 선언
```cpp
Microsoft::WRL::ComPtr<ID3D11Device> Device;
```

### 2. 참조 관리
자동.

### 3. 스마트 포인터 같은 인터페이스
`operator->`, `Get()`, `GetAddressOf()`, `Reset()` 제공.

### 4. QueryInterface — `As(&out)`
인터페이스 타입 변환. 본 프로젝트는 거의 안 쓴다.

## 프로젝트 래핑 — `COMPTR<T>`

매번 `Microsoft::WRL::ComPtr<>` 풀네임을 쓰기 번거롭고, `using namespace`는 헤더 오염이 심하다. 그래서 얇은 구조체로 한 번 더 감쌌다:

```cpp
// DirectX11_Extension/Ext_BaseHeader.h
#include <wrl/client.h>

template<typename T>
struct COMPTR
{
public:
    COMPTR() {}
    COMPTR(nullptr_t) {}

    operator Microsoft::WRL::ComPtr<T>&() { return Ptr; }
    operator T*() const                   { return Ptr.Get(); }
    T*  operator->() const                { return Ptr.Get(); }
    T** operator&()                       { return Ptr.GetAddressOf(); }
    operator bool() const                 { return Ptr != nullptr; }

    T*  Get()           const { return Ptr.Get(); }
    T** GetAddressOf()        { return Ptr.GetAddressOf(); }

    void Reset()              { Ptr.Reset(); }
    T*   Detach()             { return Ptr.Detach(); }

    template<typename U> HRESULT As(Microsoft::WRL::ComPtr<U>* out) const { return Ptr.As(out); }
    template<typename U> HRESULT As(U** out)                        const { return Ptr.CopyTo(out); }

public:
    Microsoft::WRL::ComPtr<T> Ptr = nullptr;
};
```

이 덕분에 프로젝트 코드에선 `ID3D11Device*` 대신 `COMPTR<ID3D11Device>`가 자연스럽게 쓰인다. 함수에 전달할 땐 `Get()`이나 `GetAddressOf()`로 타이밍 맞춰 넘겨주면 된다.

```cpp
// 변경 전
HRESULT Hr = D3D11CreateDevice(Adapter, ..., &Device, &Level, &Context);

// 변경 후
HRESULT Hr = D3D11CreateDevice(Adapter.Get(), ...,
    Device.GetAddressOf(), &Level, Context.GetAddressOf());
```

## 실수하기 쉬운 두 가지

### 1. 함수 인자 — **레퍼런스로 받자**

구조체(값)으로 전달하면 복사될 때마다 내부 `ComPtr`이 AddRef/Release를 부른다. 대부분의 경우 필요 없으니 레퍼런스 사용:

```cpp
// Before
void CreateRenderTargetView(ID3D11Texture2D* _Tex);

// After
void CreateRenderTargetView(COMPTR<ID3D11Texture2D>& _Tex);
```

### 2. 함수 반환 — **레퍼런스로 리턴**

값으로 리턴하면 받는 쪽에서 또 AddRef/Release가 돈다. Getter에서 특히 주의:

```cpp
// Before
ID3D11RenderTargetView* GetRTV(size_t _Index = 0) { return RTVs[_Index]; }

// After
COMPTR<ID3D11RenderTargetView>& GetRTV(size_t _Index = 0) { return RTVs[_Index]; }
```

### 3. 소유권 모델 섞지 않기

프로젝트 내 규약:
- **COM 인터페이스** → `COMPTR<>`
- **사용자 정의 리소스 클래스** → `std::shared_ptr<>`

이 둘을 섞으면 소유권 추적이 금방 꼬인다. 현재 `MainRenderTarget`은 `shared_ptr<Ext_DirectXRenderTarget>`이고, 그 안의 `Texture2D`는 `COMPTR<ID3D11Texture2D>` — 이런 식으로 레벨을 나눠 쓴다.

## 얻은 결과

- **D3D11 Live-Object 경고가 사라진다**. 종료 시 로그가 깨끗해지는 것 하나만으로도 디버깅 난이도가 크게 줄어든다.
- 예외 안전. 초기화 도중 `return` 해도 자동 Release.
- 지금까지 추가한 모든 DirectX 래퍼(Texture, RenderTarget, Shader 등)가 이 타입을 기본으로 쓴다.

## 다음 단계 / 개선거리

- **D3DReflect로 얻는 `ID3D11ShaderReflection`은 ComPtr이 아닌 원시 포인터로 주어진다** — 사용 후 `Release()`를 반드시 직접 불러야 한다. 최근 리팩토링에서 누수가 있던 걸 수정했다 ([WorkLog 2026-04-19](../WorkLog/2026-04-19.md))
- `IDXGIAdapter1` 같은 상위 인터페이스를 쓸 때 `As<>`가 유용해질 텐데, 지금은 거의 `IDXGIFactory1` 레벨 이상을 안 쓰고 있음
- Release 빌드에서도 `_CRTDBG_LEAK_CHECK_DF`가 자동으로 꺼지지 않게 신경쓰기 (의도적으로만 활성화)

## 관련 현재 코드
- [DirectX11_Extension/Ext_BaseHeader.h](../../DirectX11_Extension/Ext_BaseHeader.h) — `COMPTR` 구조체 정의
