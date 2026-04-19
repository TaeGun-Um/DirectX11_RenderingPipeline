---
title: 프레임워크 기능 추가 - 1 (디버그·수학·리소스 매니저)
original_url: https://umtimos.tistory.com/152
original_date: 2025-05-17
migrated: 2026-04-19
category: Framework
---

# 프레임워크 기능 추가 - 1

DirectX 자체를 다루기 전에, 개발이 덜 고통스럽도록 **유틸리티 기반**을 먼저 깔아둔다. 디버그 로그·수학 타입·경로 처리·리소스 관리 같은 것들. 이 한 번의 세팅이 이후 수십 개의 DirectX 래퍼에서 재사용된다.

## 디버그 클래스

디버깅 상황에서 가장 자주 찾는 세 가지 — **메시지 박스 띄우기**, **assertion**, **메모리 릭 체크** 를 묶어둔다:

```cpp
// DirectX11_Base/Base_Debug.h
#include <crtdbg.h>
#include <cassert>

#define MsgAssert(MsgText)  { std::string Text = MsgText; MessageBoxA(nullptr, Text.c_str(), "Error", MB_OK); assert(false); }
#define MsgTextBox(MsgText) { std::string Text = MsgText; MessageBoxA(nullptr, Text.c_str(), "Text",  MB_OK); }

class Base_Debug
{
public:
    static void LeakCheck()          { _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); }
    static void LeakPointBreak(int p){ _CrtSetBreakAlloc(p); }
};
```

사용 예:
```cpp
if (!IsReallyValid)
{
    MsgAssert("사전 조건 위반");
    return;
}
```

`LeakCheck()`를 메인 초기에 호출해 두면 앱 종료 시 누수된 블록이 출력창에 뜨고, `LeakPointBreak(번호)` 로 원하는 할당 번호에서 디버거 브레이크가 걸린다.

## 스마트 포인터 패턴

C++에서 "이 객체가 자기 자신을 `shared_ptr`로 반환해야 할 때" 흔히 `enable_shared_from_this`를 쓴다. 하지만 매번 `std::dynamic_pointer_cast` 쓰는 건 번거로우므로 베이스에 헬퍼를 심어둔다:

```cpp
class Ext_Object : public std::enable_shared_from_this<Ext_Object>
{
public:
    template<typename Type>
    std::shared_ptr<Type> GetSharedFromThis()
    {
        return std::dynamic_pointer_cast<Type>(shared_from_this());
    }
};
```

활용:
```cpp
void Ext_MeshComponent::PushMeshToCamera(std::string_view _CameraName)
{
    GetOwnerScene().lock()
        ->PushMeshToCamera(GetSharedFromThis<Ext_MeshComponent>(), _CameraName);
}
```

프레임워크의 모든 오브젝트가 `Ext_Object`를 상속하므로, 이후 모든 곳에서 이 패턴을 재사용한다.

## float4 — 셰이더 타입처럼 쓰기

DirectXMath의 `XMFLOAT4` / `XMVECTOR`를 union으로 묶어, HLSL 시맨틱과 매우 비슷한 느낌으로 쓸 수 있게 한다:

```cpp
class float4 final
{
public:
    float4() : x(0), y(0), z(0), w(1) {}
    float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    float4(DirectX::FXMVECTOR _V) : DirectVector(_V) {}

    union
    {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        struct { float PositionX, PositionY, SizeX, SizeY; };

        float                Arr1D[4];
        DirectX::XMFLOAT3    DirectFloat3;
        DirectX::XMFLOAT4    DirectFloat4;
        DirectX::XMVECTOR    DirectVector;
    };

    operator DirectX::FXMVECTOR() const { return DirectVector; }
    // operator+, -, *, / 등
};
```

한 타입에서 색으로도, 위치로도, 크기로도 접근할 수 있고 `XMVECTOR`로 암시 변환되어 DirectXMath 함수에 그대로 전달된다. 단점은 union을 쓰기 때문에 sizeof가 16바이트에 고정된다는 점(여러 struct 중 제일 큰 것 기준).

## float4x4 — 행렬

4×4 행렬은 `DirectX::XMMATRIX`와 `float Arr2D[4][4]`를 겹쳐놓는다:

```cpp
class float4x4 final
{
public:
    float4x4() { Identity(); }
    float4x4(DirectX::FXMMATRIX _M) : DirectMatrix(_M) {}

    union
    {
        float             Arr1D[16];
        float             Arr2D[4][4];
        float4            ArrVector[4];
        DirectX::XMMATRIX DirectMatrix;
        struct { float _00, _01, _02, _03,
                       _10, _11, _12, _13,
                       _20, _21, _22, _23,
                       _30, _31, _32, _33; };
    };
    operator DirectX::FXMMATRIX() const { return DirectMatrix; }
};
```

디버깅 중에 어떤 행 어떤 열이 뭐였는지 바로 찍어볼 수 있어 편하다.

## 문자열 / 경로 / 시간

각 파일이 하는 일은 단순하고 반복 사용되는 기능들.

- **`Base_String`** — `ToUpper`(맵 키 대소문자 통일), ANSI↔Unicode 변환 (`AnsiToUniCode` 는 `D3DCompileFromFile`에 wide string 넘길 때 씀)
- **`Base_Directory`** — `std::filesystem` 래핑. `MakePath` / `GetAllFile(확장자)` / `stem()` / `extension()` 같은 흔한 패턴 제공. 셰이더 자동 컴파일·애니메이션 로드 등에 쓰임
- **`Base_Deltatime`** — `QueryPerformanceCounter` 고해상도 타이머로 프레임 간격 측정. 하드웨어 별로 다른 성능 차이를 보정하기 위해 거의 모든 Update 함수가 delta time을 받는다

```cpp
float Base_Deltatime::TimeCheck()
{
    QueryPerformanceCounter(&Current);
    DoubleDeltaTime = (double)(Current.QuadPart - Before.QuadPart)
                    / (double)Second.QuadPart;
    floatDeltaTime  = (float)DoubleDeltaTime;
    Before.QuadPart = Current.QuadPart;
    return floatDeltaTime;
}
```

## 리소스 매니저

DirectX 리소스(텍스처·메시·셰이더·렌더타겟 등)는 종류는 많지만 **수명과 검색 패턴은 비슷**하다. "이름으로 찾기", "이름으로 등록", "단순 생성+리스트 저장" 세 가지만 잘 추상화해 두면 각 리소스 클래스가 그대로 상속받아 쓸 수 있다:

```cpp
template<typename ResourceType>
class Ext_ResourceManager
{
public:
    static std::shared_ptr<ResourceType> Find(std::string_view _Name)
    {
        std::string Upper = Base_String::ToUpper(_Name);
        auto it = NameResources.find(Upper);
        return (it == NameResources.end()) ? nullptr : it->second;
    }

    // 이름 없이 — 쉐도우처럼 자동 생성되는 익명 리소스
    static std::shared_ptr<ResourceType> CreateResource()
    {
        auto NewRes = std::make_shared<ResourceType>();
        Resources.push_back(NewRes);
        return NewRes;
    }

    // 이름으로 등록
    static std::shared_ptr<ResourceType> CreateNameResource(std::string_view _Name)
    {
        std::string Upper = Base_String::ToUpper(_Name);
        if (Find(Upper)) { MsgAssert("이미 존재하는 리소스"); return nullptr; }
        auto NewRes = std::make_shared<ResourceType>();
        NameResources.insert({ Upper, NewRes });
        return NewRes;
    }

private:
    static std::vector<std::shared_ptr<ResourceType>>             Resources;
    static std::map<std::string, std::shared_ptr<ResourceType>>   NameResources;
};
```

각 DirectX 래퍼는 이렇게 쓴다:

```cpp
class Ext_DirectXRenderTarget : public Ext_ResourceManager<Ext_DirectXRenderTarget>
{
public:
    static std::shared_ptr<Ext_DirectXRenderTarget> CreateRenderTarget(
        std::string_view _Name,
        std::shared_ptr<class Ext_DirectXTexture> _Texture,
        const float4& _Color)
    {
        auto NewRT = Ext_ResourceManager::CreateNameResource(_Name);
        NewRT->CreateRT(_Texture, _Color);
        return NewRT;
    }
};
```

덕분에 "렌더타겟을 이름으로 찾을 수 있고, 두 번 생성 시 경고가 뜨고, 프로그램 종료 시 자동 해제된다"는 요구사항이 단 한 번의 템플릿으로 해결된다.

## 얻은 결과

- DirectX 래퍼들을 만들 때 **"같은 패턴을 다시 쓰는" 중복 코드가 거의 사라진다**. 새 리소스 타입 추가는 상속 한 줄.
- 디버그 빌드에서 **메모리 누수가 자동 감지**됨.
- 셰이더 코드와 C++ 코드 사이의 간극이 좁아진다 — `float4` 하나로 양쪽 모두 납득 가능한 타입이 공용화.
- 시간 단위 갱신이 `Base_Deltatime` 하나로 표준화 — 이후 모든 Update가 `dt`를 받아서 프레임 레이트 독립적.

## 다음 단계 / 개선거리

- **리소스 매니저의 언로드** — 현재는 앱 종료 시까지 살아있다. 씬 전환 시 특정 리소스만 해제하는 API 추가 고려
- **static 초기화 순서** — 전역 static 맵이 여러 개라 TU 간 초기화 순서에 의존하지 않도록 주의 필요 (현재는 모두 cpp 한 쌍에 묶여있어 당장 문제는 없음)
- **릭 체크 루트 설정** — `Ext_Core::Run` 최상단에서 `LeakCheck()`를 호출해 기본 활성화. 의도적 디버그 할당이 있으면 `LeakPointBreak`로 국소 점검
- `float4` equality에 w 비교 누락 버그가 있었음 — 최근 수정 ([WorkLog](../WorkLog/2026-04-19.md))

## 관련 현재 코드
- [DirectX11_Base/Base_Debug.h](../../DirectX11_Base/Base_Debug.h)
- [DirectX11_Base/Base_Math.h](../../DirectX11_Base/Base_Math.h)
- [DirectX11_Base/Base_Deltatime.{h,cpp}](../../DirectX11_Base/Base_Deltatime.h)
- [DirectX11_Base/Base_String.h](../../DirectX11_Base/Base_String.h)
- [DirectX11_Base/Base_Directory.h](../../DirectX11_Base/Base_Directory.h)
- [DirectX11_Extension/Ext_ResourceManager.h](../../DirectX11_Extension/Ext_ResourceManager.h)
- [DirectX11_Extension/Ext_Object.h](../../DirectX11_Extension/Ext_Object.h)
